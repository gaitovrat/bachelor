#include "MainWindow.h"

#include <QGraphicsScene>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "./ui_MainWindow.h"
#include "Client/BaseClient.h"
#include "Client/SerialClient.h"
#include "Client/UdpClient.h"
#include "Image.h"
#include "Window/SettingsWindow.h"
#include "Utils.h"

using namespace CarQt;

MainWindow::MainWindow(const QString& name, QWidget* parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_client(nullptr),
      m_labelConnected(new QLabel("Connected", this)),
      m_labelTXBytes(new QLabel(
          QString::asprintf(MainWindow::LABEL_TX_FORMAT, 0ll), this)),
      m_labelRXBytes(new QLabel(
          QString::asprintf(MainWindow::LABEL_RX_FORMAT, 0ll), this)),
      m_recording(std::nullopt) {
    m_ui->setupUi(this);
#ifdef __APPLE__
    QMenu* fileMenu = new QMenu(name, this);
#else
    QMenu* fileMenu = new QMenu("File", this);
#endif
    QMenu* clientMenu = new QMenu("Client", this);
    QAction* preferenceAction = new QAction("Preferences", this);
    QAction* reconnectAction = new QAction("Reconnect", this);

    fileMenu->addAction(preferenceAction);
    clientMenu->addAction(reconnectAction);
    m_ui->menubar->addMenu(fileMenu);
    m_ui->menubar->addMenu(clientMenu);
    m_ui->ui_originalGraphics->setScene(new QGraphicsScene);
    m_ui->ui_normalizedView->setScene(new QGraphicsScene);
    m_ui->ui_thresholdedGraphics->setScene(new QGraphicsScene);
    

    for (QLabel* label : m_labels) m_ui->statusbar->addWidget(label);

    Settings settings = Settings::Load(SettingsWindow::FILENAME).value_or(Settings());
    m_recordingPath = settings.RecordDestination;
    updateClient(settings);
    updateConnected();

    connect(preferenceAction, &QAction::triggered, this,
            &MainWindow::openPreferences);
    connect(reconnectAction, &QAction::triggered, this,
            &MainWindow::reconnect);
    connect(m_ui->ui_recordingButton, &QPushButton::clicked, this, &MainWindow::record);
}

MainWindow::~MainWindow() {
    for (QLabel* label : m_labels) delete label;
    delete m_ui;
}

void MainWindow::update(const Shared::Data& data) {
    // Motor
    m_ui->ui_leftPwmValue->setText(QString::number(data.CarMotorData.LeftSpeed));
    m_ui->ui_rightPwmValue->setText(QString::number(data.CarMotorData.RightSpeed));

    // Servo
    m_ui->ui_servoPositionValue->setText(QString::number(data.CarSteerData.ServoPosition));
    m_ui->ui_servoAngleValue->setText(QString::number(data.CarSteerData.Angle));

    // Camera
    m_ui->ui_regsionsListSizeValue->setText(QString::number(data.CarCameraData.RegionsListSize));
    m_ui->ui_regionsCountValue->setText(QString::number(data.CarCameraData.RegionsCount));
    m_ui->ui_unchangedLeftValue->setText(Utils::ToQString(data.CarCameraData.UnchangedLeft));
    m_ui->ui_unchangedRightValue->setText(Utils::ToQString(data.CarCameraData.UnchangedRight));
    m_ui->ui_hasLeftValue->setText(Utils::ToQString(data.CarCameraData.HasLeft));
    m_ui->ui_hasRightValue->setText(Utils::ToQString(data.CarCameraData.HasRight));
    m_ui->ui_leftDistance->setText(QString::number(data.CarCameraData.LeftDistance));
    m_ui->ui_rightDistance->setText(QString::number(data.CarCameraData.RightDistance));

    m_images.emplace_front(data.CarCameraData.Line);
    int historySize = m_ui->ui_originalGraphics->size().height();
    if (m_images.size() > historySize) {
        m_images.pop_back();
    }
    cv::Mat imageOriginalBW = cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC1, cv::Scalar(0));
    cv::Mat imageNormalizedBW = cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC1, cv::Scalar(0));
    cv::Mat imageThresholdedBW = cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC1, cv::Scalar(0));

    auto imagesIterator = m_images.begin();
    for (int i = 0; i < historySize && imagesIterator != m_images.end(); ++i) {
        cv::Mat imageOriginalRow = imageOriginalBW.row(i);
        cv::Mat imageNormalizedRow = imageNormalizedBW.row(i);
        cv::Mat imageThresholdedRow = imageThresholdedBW.row(i);

        for (int j = 0; j < Image::LINE_LENGTH; ++j) {
            imageOriginalRow.at<uint8_t>(j) = static_cast<uint8_t>(imagesIterator->At(j, Image::Type::Raw));
            imageNormalizedRow.at<uint8_t>(j) = static_cast<uint8_t>(imagesIterator->At(j, Image::Type::Normalized));
            imageThresholdedRow.at<uint8_t>(j) = static_cast<uint8_t>(imagesIterator->At(j, Image::Type::Thresholded));
        }

        imageOriginalBW.row(i) = imageOriginalRow;
        imageNormalizedBW.row(i) = imageNormalizedRow;
        imageThresholdedBW.row(i) = imageThresholdedRow;
        imagesIterator++;
    }

    cv::Mat imageOriginal = cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC3, cv::Scalar(0));
    cv::Mat imageNormalized = cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC3, cv::Scalar(0));
    cv::Mat imageThresholded = cv::Mat(historySize, Image::LINE_LENGTH, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(imageOriginalBW, imageOriginal, cv::COLOR_GRAY2RGB);
    cv::cvtColor(imageNormalizedBW, imageNormalized, cv::COLOR_GRAY2RGB);
    cv::cvtColor(imageThresholdedBW, imageThresholded, cv::COLOR_GRAY2RGB);

    Utils::ShowMat(m_ui->ui_originalGraphics, imageOriginal);
    Utils::ShowMat(m_ui->ui_normalizedView, imageNormalized);
    Utils::ShowMat(m_ui->ui_thresholdedGraphics, imageThresholded);

    m_ui->ui_accelerometerXValue->setText(QString::number(data.CarSensorData.accel.X));
    m_ui->ui_accelerometerYValue->setText(QString::number(data.CarSensorData.accel.Y));
    m_ui->ui_accelerometerZValue->setText(QString::number(data.CarSensorData.accel.Z));

    m_ui->ui_gyroscopeXValue->setText(QString::number(data.CarSensorData.gyro.X));
    m_ui->ui_gyroscopeYValue->setText(QString::number(data.CarSensorData.gyro.Y));
    m_ui->ui_gyroscopeZValue->setText(QString::number(data.CarSensorData.gyro.Z));

    m_ui->ui_magnetometerXValue->setText(QString::number(data.CarSensorData.mag.X));
    m_ui->ui_magnetometerYValue->setText(QString::number(data.CarSensorData.mag.Y));
    m_ui->ui_magnetometerZValue->setText(QString::number(data.CarSensorData.mag.Z));

    if (m_recording.has_value()) {
		m_recording->Add(data);
	}
}

void MainWindow::openPreferences() {
    SettingsWindow settingsWindow(this);
    std::optional<Settings> settings = settingsWindow.execute();

    if (!settings.has_value()) {
        return;
    }

    updateClient(settings.value());
    m_recordingPath = settings->RecordDestination;
}

void MainWindow::updateClient(const Settings& settings) {
    switch (settings.AppMode) {
        case Settings::Mode::Network:
            m_client = std::make_unique<UDPClient>(settings.ClientNetwork, this);
            break;
        case Settings::Mode::Serial:
            m_client =
                std::make_unique<SerialClient>(settings.ClientSerial, this);
            break;
        default:
            break;
    }
    connect(m_client.get(), &BaseClient::DataReady, this,
        &MainWindow::update);
    connect(m_client.get(), &BaseClient::DataReceived, this,
        &MainWindow::receivedSize);
    reconnect();
}

void MainWindow::updateConnected() {
    bool connected = m_client->IsConnected();
    QString value = connected ? "Connected" : "Disconnected";

    m_labelConnected->setProperty("class", value);
    m_labelConnected->setText(value);
}

void MainWindow::reconnect() {
    m_client->Connect();
    updateConnected();
}

void MainWindow::record() {
    if (m_recording.has_value()) {
        m_recording->Save(m_recordingPath);
        m_recording = std::nullopt;
		m_ui->ui_recordingButton->setText("Start Recording");
	} else {
		m_recording = std::make_optional<Recording>();
		m_ui->ui_recordingButton->setText("Stop Recording");
	}
}

void MainWindow::receivedSize(const qint64 size) {
    m_labelRXBytes->setText(
        QString::asprintf(MainWindow::LABEL_RX_FORMAT, size));
}
