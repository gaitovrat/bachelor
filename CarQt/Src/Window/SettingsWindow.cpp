#include "SettingsWindow.h"

#include <QFileDialog>
#include <QMetaEnum>
#include <QPushButton>
#include <QSerialPort>

#include "Utils.h"
#include "ui_SettingsWindow.h"

using namespace CarQt;

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsWindow) {
    ui->setupUi(this);
    std::optional<Settings> settings = Settings::Load(SettingsWindow::FILENAME);

    if (!settings.has_value()) {
        return;
    }

    Utils::SetComboBox(*ui->cbMode, Settings::ModeToString(settings->AppMode));

    ui->leSerialPort->setText(settings->ClientSerial.PortName);
    Utils::SetComboBox(*ui->cbSerialBaudRate,
                       QString::number(settings->ClientSerial.BaudRate));
    Utils::SetComboBox(*ui->cbSerialDataBits,
                       QString::number(settings->ClientSerial.DataBits));
    QString key = QMetaEnum::fromType<QSerialPort::Parity>().valueToKey(
        settings->ClientSerial.Parity);
    key.remove("Parity");
    Utils::SetComboBox(*ui->cbSerialParity, key);
    if (settings->ClientSerial.StopBits ==
        QSerialPort::StopBits::OneAndHalfStop)
        Utils::SetComboBox(*ui->cbStopBits, "1.5");
    else
        Utils::SetComboBox(*ui->cbStopBits,
                           QString::number(settings->ClientSerial.StopBits));

    ui->leNetworkAddress->setText(settings->ClientNetwork.Address.toString());
    ui->leNetworkPort->setText(QString::number(settings->ClientNetwork.Port));

    ui->leDestination->setText(settings->RecordDestination);

    connect(ui->bChoose, &QPushButton::clicked, this,
            &SettingsWindow::selectRecordDirectory);
}

SettingsWindow::~SettingsWindow() { delete ui; }

std::optional<Settings> SettingsWindow::execute() {
    int status = exec();
    if (status == 0) {
        return std::nullopt;
    }

    Settings settings = getSettings();
    settings.Save(SettingsWindow::FILENAME);

    return settings;
}

Settings SettingsWindow::getSettings() const {
    Settings settings;
    settings.AppMode = Settings::ModeFromString(ui->cbMode->currentText());

    settings.ClientSerial.PortName = ui->leSerialPort->text();
    settings.ClientSerial.BaudRate = static_cast<QSerialPort::BaudRate>(
        ui->cbSerialBaudRate->currentText().toInt());
    settings.ClientSerial.DataBits = static_cast<QSerialPort::DataBits>(
        ui->cbSerialDataBits->currentText().toInt());
    QString text = ui->cbSerialParity->currentText() + "Parity";
    int value = QMetaEnum::fromType<QSerialPort::Parity>().keyToValue(
        text.toUtf8().constData());
    settings.ClientSerial.Parity = static_cast<QSerialPort::Parity>(value);
    float stopBits = ui->cbStopBits->currentText().toFloat();
    if (stopBits == 1.5f)
        settings.ClientSerial.StopBits = QSerialPort::StopBits::OneAndHalfStop;
    else
        settings.ClientSerial.StopBits =
            static_cast<QSerialPort::StopBits>(stopBits);

    settings.ClientNetwork.Address = QHostAddress(ui->leNetworkAddress->text());
    settings.ClientNetwork.Port = ui->leNetworkPort->text().toUInt();

    settings.RecordDestination = ui->leDestination->text();

    return settings;
}

void SettingsWindow::selectRecordDirectory() {
    QString selectedDirectory = QFileDialog::getExistingDirectory(
        this, "Select Directory", QDir::homePath());
    if (!selectedDirectory.isEmpty()) {
        ui->leDestination->setText(selectedDirectory);
    }
}
