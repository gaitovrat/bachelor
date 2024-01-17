#include "MainWindow.h"

#include "./ui_mainwindow.h"
#include "client/BaseClient.h"

#define PORT_NAME "/dev/cu.usbmodem0006210000001"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), client(PORT_NAME, this) {
    this->ui->setupUi(this);

    connect(&this->client, &BaseClient::dataReady, this, &MainWindow::update);
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