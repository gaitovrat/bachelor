#include "MainWindow.h"

#include <QtGui/qaction.h>
#include <QtWidgets/qlineedit.h>

#include "./ui_mainwindow.h"
#include "Settings.h"
#include "client/BaseClient.h"
#include "client/SerialClient.h"
#include "client/UdpClient.h"
#include "window/SettingsWindow.h"

MainWindow::MainWindow(const QString& name, QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      client(nullptr),
      labelConnected(new QLabel("Connected", this)),
      labelTXBytes(new QLabel(
          QString::asprintf(MainWindow::LABEL_BYTES_FORMAT, 0ll), this)),
      labelRXBytes(new QLabel(
          QString::asprintf(MainWindow::LABEL_BYTES_FORMAT, 0ll), this)) {
    this->ui->setupUi(this);
#ifdef __APPLE__
    QMenu* fileMenu = new QMenu(name, this);
#else
    QMenu* fileMenu = new QMenu("File", this);
#endif
    QAction* preferenceAction = new QAction("Preferences", this);

    fileMenu->addAction(preferenceAction);
    this->ui->menubar->addMenu(fileMenu);
    for (QLabel* label : this->labels) this->ui->statusbar->addWidget(label);

    std::optional<Settings> settings = Settings::load(SettingsWindow::FILENAME);
    this->updateClient(settings.value_or(Settings()));
    this->updateConnected();

    connect(this->client.get(), &BaseClient::dataReady, this,
            &MainWindow::update);
    connect(this->client.get(), &BaseClient::dataReceived, this,
            &MainWindow::receivedSize);
    connect(preferenceAction, &QAction::triggered, this,
            &MainWindow::openPreferences);
    connect(this->ui->aReconnect, &QAction::triggered, this,
            &MainWindow::reconnect);
}

MainWindow::~MainWindow() {
    for (QLabel* label : this->labels) delete label;
    delete this->ui;
}

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
            this->client =
                std::make_unique<SerialClient>(settings.serial, this);
            break;
        default:
            break;
    }
}

void MainWindow::updateConnected() {
    bool connected = this->client->connected();
    QString value = connected ? "Connected" : "Disconnected";

    this->labelConnected->setProperty("class", value);
    this->labelConnected->setText(value);
}

void MainWindow::reconnect() {
    this->client->connect();
    this->updateConnected();
}

void MainWindow::receivedSize(const qint64 size) {
    this->labelRXBytes->setText(
        QString::asprintf(MainWindow::LABEL_BYTES_FORMAT, size));
}
