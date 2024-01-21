#include "MainWindow.h"

#include "./ui_mainwindow.h"
#include "client/BaseClient.h"
#ifdef USE_UDP
#include "client/UdpClient.h"
#else
#include "client/SerialClient.h"
#endif
#include "Settings.h"
#include "window/SettingsWindow.h"

#define PORT_NAME "/dev/cu.usbmodem0006210000001"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
#ifdef USE_UDP
      client(new UDPClient(8080, this))
#else
      client(new SerialClient(PORT_NAME, this))
#endif
{
    this->ui->setupUi(this);

    connect(this->client.get(), &BaseClient::dataReady, this,
            &MainWindow::update);
    connect(this->ui->actionPreferences, &QAction::triggered, this,
            &MainWindow::openPreferences);
}

MainWindow::~MainWindow() { delete this->ui; }

void MainWindow::update(const Data& data) {
    this->ui->accelerometer_x->setText(QString::number(data.accel.x));
    this->ui->accelerometer_y->setText(QString::number(data.accel.y));
    this->ui->accelerometer_z->setText(QString::number(data.accel.z));

    this->ui->gyroscope_x->setText(QString::number(data.gyro.x));
    this->ui->gyroscope_y->setText(QString::number(data.gyro.y));
    this->ui->gyroscope_z->setText(QString::number(data.gyro.z));

    this->ui->magnetometer_x->setText(QString::number(data.mag.x));
    this->ui->magnetometer_y->setText(QString::number(data.mag.y));
    this->ui->magnetometer_z->setText(QString::number(data.mag.z));
}

void MainWindow::openPreferences() {
    SettingsWindow settingsWindow(this);
    std::optional<Settings> settings = settingsWindow.execute();

    if (settings.has_value()) {
        qInfo() << settings->portName;
    } else {
        qInfo() << "None";
    }
}
