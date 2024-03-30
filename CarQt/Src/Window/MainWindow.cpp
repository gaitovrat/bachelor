#include "MainWindow.h"

#include <QGraphicsScene>
#include <opencv2/imgproc.hpp>
#include <optional>

#include "./ui_MainWindow.h"
#include "Image.h"
#include "Shared/Signal.h"
#include "Utils.h"
#include "Window/SensorsDialog.h"
#include "Window/SettingsWindow.h"

using namespace CarQt;

MainWindow::MainWindow(const QString &name, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), client(nullptr),
      labelConnected(new QLabel("Connected", this)),
      labelJoystickConnected(new QLabel("Connected", this)),
      labelTXBytes(new QLabel(
          QString::asprintf(MainWindow::LABEL_TX_FORMAT, 0ll), this)),
      labelRXBytes(new QLabel(
          QString::asprintf(MainWindow::LABEL_RX_FORMAT, 0ll), this)),
      recording(nullptr), sensorsDialog(nullptr), timer(this) {
    this->ui->setupUi(this);
#ifdef __APPLE__
    auto *fileMenu = new QMenu(name, this);
#else
    auto *fileMenu = new QMenu("File", this);
#endif
    auto *clientMenu = new QMenu("Client", this);
    auto *viewMenu = new QMenu("View", this);
    auto *preferenceAction = new QAction("Preferences", this);
    auto *reconnectAction = new QAction("Reconnect MCU", this);
    auto *reconnectJoystickAction = new QAction("Reconnect Joystick", this);
    auto *viewSensorsChartAction = new QAction("Sensors Chart", this);

    fileMenu->addAction(preferenceAction);
    clientMenu->addAction(reconnectAction);
    clientMenu->addAction(reconnectJoystickAction);
    viewMenu->addAction(viewSensorsChartAction);
    this->ui->menubar->addMenu(fileMenu);
    this->ui->menubar->addMenu(clientMenu);
    this->ui->menubar->addMenu(viewMenu);
    this->ui->ui_originalGraphics->setScene(new QGraphicsScene);
    this->ui->ui_normalizedView->setScene(new QGraphicsScene);
    this->ui->ui_thresholdedGraphics->setScene(new QGraphicsScene);

    for (QLabel *label : labels)
        this->ui->statusbar->addWidget(label);

    this->timer.setInterval(10);

    Settings settings = Settings::load(SettingsWindow::FILENAME);

    updateClient(settings);
    updateJoystick();

    connect(this->client, &UDPClient::dataReady, this, &MainWindow::update);
    connect(this->client, &UDPClient::dataReceived, this,
            &MainWindow::receivedSize);
    connect(preferenceAction, &QAction::triggered, this,
            &MainWindow::openPreferences);
    connect(reconnectAction, &QAction::triggered, this, &MainWindow::reconnect);
    connect(this->ui->ui_recordingButton, &QPushButton::clicked, this,
            &MainWindow::record);
    connect(&this->timer, &QTimer::timeout, this, &MainWindow::handleJoystick);
    connect(reconnectJoystickAction, &QAction::triggered, this,
            &MainWindow::updateJoystick);
    connect(viewSensorsChartAction, &QAction::triggered, this,
            &MainWindow::openSensorsDialog);

    connect(this->ui->bStartCar, &QPushButton::clicked, this,
            &MainWindow::startCar);

    //    this->timer.start();
}

MainWindow::~MainWindow() {
    this->timer.stop();

    for (QLabel *label : labels)
        delete label;

    if (client) {
        delete client;
    }

    if (recording) {
        delete recording;
    }

    delete ui;
}

void MainWindow::update(const Shared::Data &data) {
    // Motor
    this->ui->ui_leftPwmValue->setText(
        QString::number(data.motorData.leftSpeed));
    this->ui->ui_rightPwmValue->setText(
        QString::number(data.motorData.rightSpeed));

    // Servo
    this->ui->ui_servoPositionValue->setText(
        QString::number(data.steerData.servoPosition));
    this->ui->ui_servoAngleValue->setText(
        QString::number(data.steerData.angle));

    // Camera
    this->ui->ui_regsionsListSizeValue->setText(
        QString::number(data.cameraData.regionsListSize));
    this->ui->ui_regionsCountValue->setText(
        QString::number(data.cameraData.regionsCount));
    this->ui->ui_unchangedLeftValue->setText(
        Utils::toQString(data.cameraData.unchangedLeft));
    this->ui->ui_unchangedRightValue->setText(
        Utils::toQString(data.cameraData.unchangedRight));
    this->ui->ui_hasLeftValue->setText(
        Utils::toQString(data.cameraData.hasLeft));
    this->ui->ui_hasRightValue->setText(
        Utils::toQString(data.cameraData.hasRight));
    this->ui->ui_leftDistance->setText(
        QString::number(data.cameraData.leftDistance));
    this->ui->ui_rightDistance->setText(
        QString::number(data.cameraData.rightDistance));

    images.emplace_front(data.cameraData.line);
    int historySize = this->ui->ui_originalGraphics->size().height();
    if (images.size() > historySize) {
        images.pop_back();
    }
    cv::Mat imageOriginalBW =
        cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC1, cv::Scalar(0));
    cv::Mat imageNormalizedBW =
        cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC1, cv::Scalar(0));
    cv::Mat imageThresholdedBW =
        cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC1, cv::Scalar(0));

    auto imagesIterator = images.begin();
    for (int i = 0; i < historySize && imagesIterator != images.end(); ++i) {
        cv::Mat imageOriginalRow = imageOriginalBW.row(i);
        cv::Mat imageNormalizedRow = imageNormalizedBW.row(i);
        cv::Mat imageThresholdedRow = imageThresholdedBW.row(i);

        for (int j = 0; j < Image::LINE_LENGTH; ++j) {
            imageOriginalRow.at<uint8_t>(j) =
                static_cast<uint8_t>(imagesIterator->at(j, Image::Type::Raw));
            imageNormalizedRow.at<uint8_t>(j) = static_cast<uint8_t>(
                imagesIterator->at(j, Image::Type::Normalized));
            imageThresholdedRow.at<uint8_t>(j) = static_cast<uint8_t>(
                imagesIterator->at(j, Image::Type::Thresholded));
        }

        imageOriginalBW.row(i) = imageOriginalRow;
        imageNormalizedBW.row(i) = imageNormalizedRow;
        imageThresholdedBW.row(i) = imageThresholdedRow;
        imagesIterator++;
    }

    cv::Mat imageOriginal =
        cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC3, cv::Scalar(0));
    cv::Mat imageNormalized =
        cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC3, cv::Scalar(0));
    cv::Mat imageThresholded =
        cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(imageOriginalBW, imageOriginal, cv::COLOR_GRAY2RGB);
    cv::cvtColor(imageNormalizedBW, imageNormalized, cv::COLOR_GRAY2RGB);
    cv::cvtColor(imageThresholdedBW, imageThresholded, cv::COLOR_GRAY2RGB);

    Utils::showMat(this->ui->ui_originalGraphics, imageOriginal);
    Utils::showMat(this->ui->ui_normalizedView, imageNormalized);
    Utils::showMat(this->ui->ui_thresholdedGraphics, imageThresholded);

    this->ui->ui_accelerometerXValue->setText(
        QString::number(data.sensorData.accel.x));
    this->ui->ui_accelerometerYValue->setText(
        QString::number(data.sensorData.accel.y));
    this->ui->ui_accelerometerZValue->setText(
        QString::number(data.sensorData.accel.z));

    this->ui->ui_gyroscopeXValue->setText(
        QString::number(data.sensorData.gyro.x));
    this->ui->ui_gyroscopeYValue->setText(
        QString::number(data.sensorData.gyro.y));
    this->ui->ui_gyroscopeZValue->setText(
        QString::number(data.sensorData.gyro.z));

    this->ui->ui_magnetometerXValue->setText(
        QString::number(data.sensorData.mag.x));
    this->ui->ui_magnetometerYValue->setText(
        QString::number(data.sensorData.mag.y));
    this->ui->ui_magnetometerZValue->setText(
        QString::number(data.sensorData.mag.z));

    if (recording) {
        recording->add(data);
    }

    if (sensorsDialog) {
        sensorsDialog->add(data.sensorData, data.timestamp);
    }
}

void MainWindow::openPreferences() {
    SettingsWindow settingsWindow(this);
    std::optional<Settings> settings = settingsWindow.execute();

    if (!settings.has_value()) {
        return;
    }

    updateClient(*settings);
}

void MainWindow::updateClient(const Settings &settings) {
    this->recordingPath = settings.recordDestination;
    if (this->client) {
        delete this->client;
    }
    this->client = new UDPClient(settings.network, this);
    reconnect();
}

void MainWindow::reconnect() {
    this->client->bind();

    bool connected = this->client->isConnected();
    QString value = connected ? "Connected" : "Disconnected";

    labelConnected->setProperty("class", value);
    labelConnected->setText(value);
}

void MainWindow::record() {
    if (recording) {
        recording->save(recordingPath);
        delete recording;
        recording = nullptr;
        this->ui->ui_recordingButton->setText("Start Recording");
    } else {
        recording = new Recording();
        this->ui->ui_recordingButton->setText("Stop Recording");
    }
}

void MainWindow::receivedSize(const qint64 size) const {
    labelRXBytes->setText(QString::asprintf(MainWindow::LABEL_RX_FORMAT, size));
}

void MainWindow::updateJoystick() {
    bool opened = this->joystick.opened();

    if (!opened) {
        opened = this->joystick.open();
    }

    QString qssClass = opened ? "Connected" : "Disconnected";
    QString text = opened ? this->joystick.getName() : "No gamepad found";

    this->labelJoystickConnected->setProperty("class", qssClass);
    this->labelJoystickConnected->setText(text);
}

void MainWindow::handleJoystick() const {
    Shared::Signal signal = this->joystick.getSignal();
    qDebug() << "Joystick signal: " << signal.type << " " << signal.value;
}

void MainWindow::destroySensorsDialog() {
    if (!this->sensorsDialog) {
        return;
    }

    this->sensorsDialog->deleteLater();
    this->sensorsDialog = nullptr;
}

void MainWindow::openSensorsDialog() {
    if (this->sensorsDialog) {
        return;
    }

    this->sensorsDialog = new SensorsDialog(this);

    connect(this->sensorsDialog, &SensorsDialog::finished, this,
            &MainWindow::destroySensorsDialog);

    sensorsDialog->show();
}
void MainWindow::startCar() {
    QString buttonText = this->ui->bStartCar->text();
    Shared::Signal signal;

    if (buttonText == "Start") {
        this->ui->bStartCar->setText("Stop");
        signal.type = Shared::SignalType::START;
    } else {
        this->ui->bStartCar->setText("Start");
        signal.type = Shared::SignalType::STOP;
    }

    this->client->send(signal);
}
