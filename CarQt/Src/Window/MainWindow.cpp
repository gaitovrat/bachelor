#include "MainWindow.h"

#include <QtGui/qaction.h>
#include <QtWidgets/qlineedit.h>

#include "./ui_MainWindow.h"
#include "Settings.h"
#include "Client/BaseClient.h"
#include "Client/SerialClient.h"
#include "Client/UdpClient.h"
#include "Window/SettingsWindow.h"

MainWindow::MainWindow(const QString& name, QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      client(nullptr),
      labelConnected(new QLabel("Connected", this)),
      labelTXBytes(new QLabel(
          QString::asprintf(MainWindow::LABEL_TX_FORMAT, 0ll), this)),
      labelRXBytes(new QLabel(
          QString::asprintf(MainWindow::LABEL_RX_FORMAT, 0ll), this)),
      recording(std::nullopt) {
    this->ui->setupUi(this);
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
    this->ui->menubar->addMenu(fileMenu);
    this->ui->menubar->addMenu(clientMenu);

    for (QLabel* label : this->labels) this->ui->statusbar->addWidget(label);

    Settings settings = Settings::load(SettingsWindow::FILENAME).value_or(Settings());
    this->recordingPath = settings.recordDestination;
    this->updateClient(settings);
    this->updateConnected();

    connect(preferenceAction, &QAction::triggered, this,
            &MainWindow::openPreferences);
    connect(reconnectAction, &QAction::triggered, this,
            &MainWindow::reconnect);
    connect(this->ui->bRecord, &QPushButton::clicked, this, &MainWindow::record);
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

    if (this->recording.has_value()) {
		this->recording->add(data);
	}
}

void MainWindow::openPreferences() {
    SettingsWindow settingsWindow(this);
    std::optional<Settings> settings = settingsWindow.execute();

    if (!settings.has_value()) {
        return;
    }

    this->updateClient(settings.value());
    this->recordingPath = settings->recordDestination;
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
    connect(this->client.get(), &BaseClient::dataReady, this,
        &MainWindow::update);
    connect(this->client.get(), &BaseClient::dataReceived, this,
        &MainWindow::receivedSize);
    this->reconnect();
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

void MainWindow::record() {
    if (this->recording.has_value()) {
        this->recording->save(this->recordingPath);
        this->recording = std::nullopt;
		this->ui->bRecord->setText("Start Recording");
	} else {
		this->recording = std::make_optional<Recording>();
		this->ui->bRecord->setText("Stop Recording");
	}
}

void MainWindow::receivedSize(const qint64 size) {
    this->labelRXBytes->setText(
        QString::asprintf(MainWindow::LABEL_RX_FORMAT, size));
}
