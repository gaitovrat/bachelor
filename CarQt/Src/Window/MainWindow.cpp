#include "MainWindow.h"

#include <QGraphicsScene>
#include <opencv2/imgproc.hpp>
#include <optional>

#include "./ui_MainWindow.h"
#include "Image.h"
#include "Utils.h"
#include "Window/SettingsWindow.h"

using namespace CarQt;

MainWindow::MainWindow(const QString &name, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), client(nullptr),
      labelConnected(new QLabel("Connected", this)),
      labelTXBytes(new QLabel(
          QString::asprintf(MainWindow::LABEL_TX_FORMAT, 0ll), this)),
      labelRXBytes(new QLabel(
          QString::asprintf(MainWindow::LABEL_RX_FORMAT, 0ll), this)),
      recording(nullptr) {
    this->ui->setupUi(this);
#ifdef __APPLE__
    auto *fileMenu = new QMenu(name, this);
#else
    auto *fileMenu = new QMenu("File", this);
#endif
    auto *clientMenu = new QMenu("Client", this);
    auto *preferenceAction = new QAction("Preferences", this);
    auto *reconnectAction = new QAction("Reconnect MCU", this);

    fileMenu->addAction(preferenceAction);
    clientMenu->addAction(reconnectAction);
    this->ui->menubar->addMenu(fileMenu);
    this->ui->menubar->addMenu(clientMenu);
    this->ui->ui_originalGraphics->setScene(new QGraphicsScene);
    this->ui->ui_normalizedView->setScene(new QGraphicsScene);
    this->ui->ui_thresholdedGraphics->setScene(new QGraphicsScene);

    this->ui->statusbar->addWidget(labelConnected);
    this->ui->statusbar->addWidget(labelRXBytes);
    this->ui->statusbar->addWidget(labelTXBytes);

    Settings settings = Settings::load(SettingsWindow::FILENAME);

    updateClient(settings);

    connect(preferenceAction, &QAction::triggered, this,
            &MainWindow::openPreferences);
    connect(reconnectAction, &QAction::triggered, this, &MainWindow::reconnect);
    connect(this->ui->ui_recordingButton, &QPushButton::clicked, this,
            &MainWindow::record);
}

MainWindow::~MainWindow() {
    delete labelConnected;
    delete labelRXBytes;
    delete labelTXBytes;
    delete client;
    delete recording;
    delete ui;
}

void MainWindow::update(const Shared::Data &data) {
    // Motor
    this->ui->ui_leftPwmValue->setText(QString::number(data.leftSpeed));
    this->ui->ui_rightPwmValue->setText(QString::number(data.rightSpeed));

    // Servo
    this->ui->ui_servoPositionValue->setText(
        QString::number(data.servoPosition));
    this->ui->ui_servoAngleValue->setText(QString::number(data.angle));

    // Camera
    this->ui->ui_regsionsListSizeValue->setText(
        QString::number(data.regionsListSize));
    this->ui->ui_regionsCountValue->setText(QString::number(data.regionsCount));
    this->ui->ui_unchangedLeftValue->setText(
        Utils::toQString(data.unchangedLeft));
    this->ui->ui_unchangedRightValue->setText(
        Utils::toQString(data.unchangedRight));
    this->ui->ui_hasLeftValue->setText(Utils::toQString(data.hasLeft));
    this->ui->ui_hasRightValue->setText(Utils::toQString(data.hasRight));
    this->ui->ui_leftDistance->setText(QString::number(data.leftDistance));
    this->ui->ui_rightDistance->setText(QString::number(data.rightDistance));

    images.emplace_front(data.line);
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

    this->ui->ui_accelerometerXValue->setText(QString::number(data.accel.x));
    this->ui->ui_accelerometerYValue->setText(QString::number(data.accel.y));
    this->ui->ui_accelerometerZValue->setText(QString::number(data.accel.z));

    this->ui->ui_gyroscopeXValue->setText(QString::number(data.gyro.x));
    this->ui->ui_gyroscopeYValue->setText(QString::number(data.gyro.y));
    this->ui->ui_gyroscopeZValue->setText(QString::number(data.gyro.z));

    this->ui->ui_magnetometerXValue->setText(QString::number(data.mag.x));
    this->ui->ui_magnetometerYValue->setText(QString::number(data.mag.y));
    this->ui->ui_magnetometerZValue->setText(QString::number(data.mag.z));

    if (recording) {
        recording->add(data);
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
    delete this->client;

    this->client = new UDPClient(settings.network, this);
    reconnect();
}

void MainWindow::reconnect() {
    this->client->bind();

    bool connected = this->client->isConnected();
    QString value = connected ? "Connected" : "Disconnected";

    labelConnected->setProperty("class", value);
    labelConnected->setText(value);

    connect(this->client, &UDPClient::dataReady, this, &MainWindow::update);
    connect(this->client, &UDPClient::dataReceived, this,
            &MainWindow::receivedSize);
    connect(this->client, &UDPClient::dataTransmitted, this,
            &MainWindow::transmitSize);
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

void MainWindow::transmitSize(qint64 size) const {
    labelTXBytes->setText(QString::asprintf(MainWindow::LABEL_TX_FORMAT, size));
}
