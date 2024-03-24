#include "MainWindow.h"

#include <QGraphicsScene>
#include <opencv2/imgproc.hpp>

#include "./ui_MainWindow.h"
#include "Client/BaseClient.h"
#include "Client/SerialClient.h"
#include "Client/UdpClient.h"
#include "Image.h"
#include "Shared/Signal.h"
#include "Utils.h"
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
      recording(std::nullopt), timer(this) {
    ui->setupUi(this);
#ifdef __APPLE__
    auto *fileMenu = new QMenu(name, this);
#else
    auto *fileMenu = new QMenu("File", this);
#endif
    auto *clientMenu = new QMenu("Client", this);
    auto *preferenceAction = new QAction("Preferences", this);
    auto *reconnectAction = new QAction("Reconnect MCU", this);
    auto *reconnectJoystickAction = new QAction("Reconnect Joystick", this);

    fileMenu->addAction(preferenceAction);
    clientMenu->addAction(reconnectAction);
    clientMenu->addAction(reconnectJoystickAction);
    ui->menubar->addMenu(fileMenu);
    ui->menubar->addMenu(clientMenu);
    ui->ui_originalGraphics->setScene(new QGraphicsScene);
    ui->ui_normalizedView->setScene(new QGraphicsScene);
    ui->ui_thresholdedGraphics->setScene(new QGraphicsScene);

    for (QLabel *label : labels)
        ui->statusbar->addWidget(label);

    this->timer.setInterval(10);

    Settings settings =
        Settings::Load(SettingsWindow::FILENAME).value_or(Settings());
    recordingPath = settings.RecordDestination;
    updateClient(settings);
    updateConnected();
    this->updateJoystick();

    connect(preferenceAction, &QAction::triggered, this,
            &MainWindow::openPreferences);
    connect(reconnectAction, &QAction::triggered, this, &MainWindow::reconnect);
    connect(ui->ui_recordingButton, &QPushButton::clicked, this,
            &MainWindow::record);
    connect(&this->timer, &QTimer::timeout, this, &MainWindow::handleJoystick);
    connect(reconnectJoystickAction, &QAction::triggered, this,
            &MainWindow::updateJoystick);

    this->timer.start();
}

MainWindow::~MainWindow() {
    this->timer.stop();
    for (QLabel *label : labels)
        delete label;
    delete ui;
}

void MainWindow::update(const Shared::Data &data) {
    // Motor
    ui->ui_leftPwmValue->setText(QString::number(data.CarMotorData.LeftSpeed));
    ui->ui_rightPwmValue->setText(
        QString::number(data.CarMotorData.RightSpeed));

    // Servo
    ui->ui_servoPositionValue->setText(
        QString::number(data.CarSteerData.ServoPosition));
    ui->ui_servoAngleValue->setText(QString::number(data.CarSteerData.Angle));

    // Camera
    ui->ui_regsionsListSizeValue->setText(
        QString::number(data.CarCameraData.RegionsListSize));
    ui->ui_regionsCountValue->setText(
        QString::number(data.CarCameraData.RegionsCount));
    ui->ui_unchangedLeftValue->setText(
        Utils::ToQString(data.CarCameraData.UnchangedLeft));
    ui->ui_unchangedRightValue->setText(
        Utils::ToQString(data.CarCameraData.UnchangedRight));
    ui->ui_hasLeftValue->setText(Utils::ToQString(data.CarCameraData.HasLeft));
    ui->ui_hasRightValue->setText(
        Utils::ToQString(data.CarCameraData.HasRight));
    ui->ui_leftDistance->setText(
        QString::number(data.CarCameraData.LeftDistance));
    ui->ui_rightDistance->setText(
        QString::number(data.CarCameraData.RightDistance));

    images.emplace_front(data.CarCameraData.Line);
    int historySize = ui->ui_originalGraphics->size().height();
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
                static_cast<uint8_t>(imagesIterator->At(j, Image::Type::Raw));
            imageNormalizedRow.at<uint8_t>(j) = static_cast<uint8_t>(
                imagesIterator->At(j, Image::Type::Normalized));
            imageThresholdedRow.at<uint8_t>(j) = static_cast<uint8_t>(
                imagesIterator->At(j, Image::Type::Thresholded));
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

    Utils::ShowMat(ui->ui_originalGraphics, imageOriginal);
    Utils::ShowMat(ui->ui_normalizedView, imageNormalized);
    Utils::ShowMat(ui->ui_thresholdedGraphics, imageThresholded);

    ui->ui_accelerometerXValue->setText(
        QString::number(data.CarSensorData.accel.X));
    ui->ui_accelerometerYValue->setText(
        QString::number(data.CarSensorData.accel.Y));
    ui->ui_accelerometerZValue->setText(
        QString::number(data.CarSensorData.accel.Z));

    ui->ui_gyroscopeXValue->setText(QString::number(data.CarSensorData.gyro.X));
    ui->ui_gyroscopeYValue->setText(QString::number(data.CarSensorData.gyro.Y));
    ui->ui_gyroscopeZValue->setText(QString::number(data.CarSensorData.gyro.Z));

    ui->ui_magnetometerXValue->setText(
        QString::number(data.CarSensorData.mag.X));
    ui->ui_magnetometerYValue->setText(
        QString::number(data.CarSensorData.mag.Y));
    ui->ui_magnetometerZValue->setText(
        QString::number(data.CarSensorData.mag.Z));

    if (recording.has_value()) {
        recording->Add(data);
    }
}

void MainWindow::openPreferences() {
    SettingsWindow settingsWindow(this);
    std::optional<Settings> settings = settingsWindow.execute();

    if (!settings.has_value()) {
        return;
    }

    updateClient(settings.value());
    recordingPath = settings->RecordDestination;
}

void MainWindow::updateClient(const Settings &settings) {
    switch (settings.AppMode) {
    case Settings::Mode::Network:
        client = std::make_unique<UDPClient>(settings.ClientNetwork, this);
        break;
    case Settings::Mode::Serial:
        client = std::make_unique<SerialClient>(settings.ClientSerial, this);
        break;
    default:
        break;
    }
    connect(client.get(), &BaseClient::DataReady, this, &MainWindow::update);
    connect(client.get(), &BaseClient::DataReceived, this,
            &MainWindow::receivedSize);
    reconnect();
}

void MainWindow::updateConnected() {
    bool connected = client->IsConnected();
    QString value = connected ? "Connected" : "Disconnected";

    labelConnected->setProperty("class", value);
    labelConnected->setText(value);
}

void MainWindow::reconnect() {
    client->Connect();
    updateConnected();
}

void MainWindow::record() {
    if (recording.has_value()) {
        recording->Save(recordingPath);
        recording = std::nullopt;
        ui->ui_recordingButton->setText("Start Recording");
    } else {
        recording = std::make_optional<Recording>();
        ui->ui_recordingButton->setText("Stop Recording");
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
