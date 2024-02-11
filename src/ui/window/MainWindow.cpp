#include "MainWindow.h"

#include "./ui_mainwindow.h"
#include "Settings.h"
#include "client/BaseClient.h"
#include "client/UdpClient.h"
#include "client/SerialClient.h"
#include "window/SettingsWindow.h"
#include <QtGui/qaction.h>

MainWindow::MainWindow(const QString& name, QWidget* parent)
    : QMainWindow(parent),
      client(nullptr),
      ui(new Ui::MainWindow) {
#ifdef __APPLE__
    QMenu *fileMenu = new QMenu(name, this);
#else
    QMenu *fileMenu = new QMenu("File", this);
#endif
    QAction *preferenceAction = new QAction("Preferences", this);

    this->ui->setupUi(this);

    fileMenu->addAction(preferenceAction);
    this->ui->menubar->addMenu(fileMenu);

    std::optional<Settings> settings = Settings::load(SettingsWindow::FILENAME);
    this->updateClient(settings.value_or(Settings()));

    connect(this->client.get(), &BaseClient::dataReady, this,
            &MainWindow::update);
    connect(preferenceAction, &QAction::triggered, this,
            &MainWindow::openPreferences);
    connect(this->ui->aReconnect, &QAction::triggered, this, 
            &MainWindow::reconnect);
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

    if (!settings.has_value()) {
        return;
    }

    this->updateClient(settings.value());
}

 void MainWindow::updateClient(const Settings& settings) {
    switch (settings.mode) {
    case Settings::Mode::Network:
        this->client = std::make_unique<UDPClient>(settings.network, this);
        break;
    case Settings::Mode::Serial:
        this->client = std::make_unique<SerialClient>(settings.serial, this);
        break;
    default:
        break;
    }
}

void MainWindow::updateConnected() {
    bool connected = this->client->connected();
    QString value = connected ? "Connected" : "Disconnected";

    this->ui->lConnected->setProperty("class", value);
    this->ui->lConnected->setText(value);
}

void MainWindow::reconnect() {
    this->client->connect();
    this->updateConnected();
}
