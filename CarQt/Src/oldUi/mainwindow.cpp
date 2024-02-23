#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "Utils.h"
#include "common/Mode.h"

#include <cstdint>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

static constexpr int PORT = 8080;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
    m_udp(std::make_shared<UDPClient>(PORT, parent)), m_gamecontroller(nullptr), m_timer(this)  {
    ui->setupUi(this);
    ui->ui_rideChooseComboBox->addItem("Ride Automatic", QVariant(::Car::Mode::RideDefault));
    ui->ui_rideChooseComboBox->addItem("Ride Manual", QVariant(::Car::Mode::RideManual));
    ui->ui_rideChooseComboBox->setCurrentIndex(::Car::Mode::RideDefault);

    ui->ui_originalGraphics->setScene(new QGraphicsScene);
    ui->ui_normalizedView->setScene(new QGraphicsScene);
    ui->ui_thresholdedGraphics->setScene(new QGraphicsScene);

    m_timer.setInterval(10);

    SDL_Init(SDL_INIT_GAMECONTROLLER);
    int nJoysticks = SDL_NumJoysticks();
    if (nJoysticks < 1) {
        qWarning("No joystick connected");
    } else {
        m_gamecontroller = SDL_GameControllerOpen(0);
    }

    connect(m_udp.get(), &UDPClient::dataReady, this, &MainWindow::DataReady);
    connect(ui->ui_openPIDButton, &QPushButton::clicked, this, &MainWindow::OpenPID);
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::HandleJoystick);

    m_timer.start();
}

MainWindow::~MainWindow()
{
    m_timer.stop();
    if (m_gamecontroller) {
        SDL_GameControllerClose(m_gamecontroller);
    }

    delete ui->ui_originalGraphics->scene();
    delete ui->ui_normalizedView->scene();
    delete ui->ui_thresholdedGraphics->scene();
    delete ui;
}

void MainWindow::DataReady(UdpData &udpData) {
    m_dataHistory.push_front(udpData);

    // Motor
    ui->ui_leftPwmValue->setText(QString::number(udpData.LeftSpeed));
    ui->ui_rightPwmValue->setText(QString::number(udpData.RightSpeed));

    // Servo
    ui->ui_servoPositionValue->setText(QString::number(udpData.ServoPosition));
    ui->ui_angleValue->setText(QString::number(udpData.Angle));

    // Ride
    ui->ui_rideChooseComboBox->setCurrentIndex(udpData.Mode);

    // Camera
    ui->ui_regsionsListSizeValue->setText(QString::number(udpData.RegionsListSize));
    ui->ui_regionsCountValue->setText(QString::number(udpData.RegionsCount));
    ui->ui_unchangedLeftValue->setText(Utils::ToQString(udpData.UnchangedLeft));
    ui->ui_unchangedRightValue->setText(Utils::ToQString(udpData.UnchangedRight));
    ui->ui_hasLeftValue->setText(Utils::ToQString(udpData.HasLeft));
    ui->ui_hasRightValue->setText(Utils::ToQString(udpData.HasRight));
    ui->ui_leftDistance->setText(QString::number(udpData.LeftDistance));
    ui->ui_rightDistance->setText(QString::number(udpData.RightDistance));

    m_images.emplace_front(udpData.Line);
    int historySize = ui->ui_originalGraphics->size().height();
    if (m_images.size() > historySize) {
        m_images.pop_back();
        m_dataHistory.pop_back();
    }
    cv::Mat imageOriginalBW = cv::Mat(historySize, TFC_CAMERA_LINE_LENGTH, CV_8UC1, cv::Scalar(0));
    cv::Mat imageNormalizedBW = cv::Mat(historySize, TFC_CAMERA_LINE_LENGTH, CV_8UC1, cv::Scalar(0));
    cv::Mat imageThresholdedBW = cv::Mat(historySize, TFC_CAMERA_LINE_LENGTH, CV_8UC1, cv::Scalar(0));

    auto imagesIterator = m_images.begin();
    for (int i = 0; i < historySize && imagesIterator != m_images.end(); ++i) {
        cv::Mat imageOriginalRow = imageOriginalBW.row(i);
        cv::Mat imageNormalizedRow = imageNormalizedBW.row(i);
        cv::Mat imageThresholdedRow = imageThresholdedBW.row(i);

        for (int j = 0; j < TFC_CAMERA_LINE_LENGTH; ++j) {
            imageOriginalRow.at<uint8_t>(j) = static_cast<uint8_t>(imagesIterator->At(j, ImgType::Raw));
            imageNormalizedRow.at<uint8_t>(j) = static_cast<uint8_t>(imagesIterator->At(j, ImgType::Normalized));
            imageThresholdedRow.at<uint8_t>(j) = static_cast<uint8_t>(imagesIterator->At(j, ImgType::Thresholded));
        }

        imageOriginalBW.row(i) = imageOriginalRow;
        imageNormalizedBW.row(i) = imageNormalizedRow;
        imageThresholdedBW.row(i) = imageThresholdedRow;
        imagesIterator++;
    }

    cv::Mat imageOriginal = cv::Mat(historySize, TFC_CAMERA_LINE_LENGTH, CV_8UC3, cv::Scalar(0));
    cv::Mat imageNormalized = cv::Mat(historySize, TFC_CAMERA_LINE_LENGTH, CV_8UC3, cv::Scalar(0));
    cv::Mat imageThresholded = cv::Mat(historySize, TFC_CAMERA_LINE_LENGTH, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(imageOriginalBW, imageOriginal, cv::COLOR_GRAY2RGB);
    cv::cvtColor(imageNormalizedBW, imageNormalized, cv::COLOR_GRAY2RGB);
    cv::cvtColor(imageThresholdedBW, imageThresholded, cv::COLOR_GRAY2RGB);

    ::Utils::ShowMat(ui->ui_originalGraphics, imageOriginal);
    ::Utils::ShowMat(ui->ui_normalizedView, imageNormalized);
    ::Utils::ShowMat(ui->ui_thresholdedGraphics, imageThresholded);
}

void MainWindow::OpenPID(bool) {
    if (!m_pidWindow) {
        m_pidWindow = std::make_unique<PIDWindow>(m_udp);
    }

    if (m_pidWindow->isVisible()) {
        return;
    }
    m_pidWindow->show();
}

void MainWindow::HandleJoystick() {
    if (!m_gamecontroller) {
        qWarning() << "No game controller found";
        return;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_CONTROLLERAXISMOTION) {
            int button = event.caxis.axis;
            if (button == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
                qInfo() << "Speed up " << static_cast<double>(event.caxis.value) / std::numeric_limits<Sint16>::max();
            } else if (button == SDL_CONTROLLER_AXIS_LEFTX) {
                qInfo() << "Turn " << event.caxis.value;
            }
        }
    }
}
