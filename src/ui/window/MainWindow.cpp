#include "MainWindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    this->ui->setupUi(this);

    this->ui->accelerometer_x->setText(QString::number(.0f));
    this->ui->accelerometer_y->setText(QString::number(.1f));
    this->ui->accelerometer_z->setText(QString::number(.2f));

    this->ui->gyroscope_x->setText(QString::number(1.f));
    this->ui->gyroscope_y->setText(QString::number(1.1f));
    this->ui->gyroscope_z->setText(QString::number(1.2f));

    this->ui->magnetometer_x->setText(QString::number(2.f));
    this->ui->magnetometer_y->setText(QString::number(2.1f));
    this->ui->magnetometer_z->setText(QString::number(2.2f));
}

MainWindow::~MainWindow() { delete this->ui; }
