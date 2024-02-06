#include "MainWindow.h"

#include "./ui_mainwindow.h"
#include "client/BaseClient.h"
#include <QtGui/qaction.h>
#include <QtWidgets/qmenu.h>
#include "client/UdpClient.h"
#include "client/SerialClient.h"
#include "Settings.h"
#include "window/SettingsWindow.h"

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
    Settings::Mode mode = Settings::Mode::Serial;
    if (settings.has_value()) {
        mode = settings->mode;
    }

    switch (mode) {
    case Settings::Mode::Network:
        this->client = std::make_unique<UDPClient>(settings->network, this);
        break;
    case Settings::Mode::Serial:
        this->client = std::make_unique<SerialClient>(settings->serial, this);
        break;
    default:
        break;
    }

    connect(this->client.get(), &BaseClient::dataReady, this,
            &MainWindow::update);
    connect(preferenceAction, &QAction::triggered, this,
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
        qInfo() << "Ok";
    } else {
        qInfo() << "None";
    }
}
