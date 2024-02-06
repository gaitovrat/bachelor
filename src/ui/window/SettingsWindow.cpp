#include "SettingsWindow.h"

#include <QMetaEnum>
#include <QSerialPort>
#include <QFileDialog>
#include <QPushButton>

#include "Utils.h"
#include "Settings.h"
#include "ui_SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsWindow) {
    ui->setupUi(this);
    std::optional<Settings> settings = Settings::load(SettingsWindow::FILENAME);

    if (!settings.has_value()) {
        return;
    }

    Utils::setComboBox(*this->ui->cbMode, Settings::modeToString(settings->mode));

    this->ui->leSerialPort->setText(settings->serial.portName);
    Utils::setComboBox(*this->ui->cbSerialBaudRate, QString::number(settings->serial.baudRate));
    Utils::setComboBox(*this->ui->cbSerialDataBits, QString::number(settings->serial.dataBits));
    QString key = QMetaEnum::fromType<QSerialPort::Parity>().valueToKey(settings->serial.parity);
    key.remove("Parity");
    Utils::setComboBox(*this->ui->cbSerialParity, key);
    if (settings->serial.stopBits == QSerialPort::StopBits::OneAndHalfStop)
        Utils::setComboBox(*this->ui->cbStopBits, "1.5");
    else
        Utils::setComboBox(*this->ui->cbStopBits, QString::number(settings->serial.stopBits));

    this->ui->leNetworkAddress->setText(settings->network.address);
    this->ui->leNetworkPort->setText(QString::number(settings->network.port));

    this->ui->leDestination->setText(settings->recordDestination);

    connect(this->ui->bChoose, &QPushButton::clicked, this, &SettingsWindow::selectRecordDirectory);
}

SettingsWindow::~SettingsWindow() { delete ui; }

std::optional<Settings> SettingsWindow::execute() {
    int status = this->exec();
    if (status == 0) {
        return std::nullopt;
    }

    Settings settings = getSettings();
    settings.save(SettingsWindow::FILENAME);

    return settings;
}

Settings SettingsWindow::getSettings() {
    Settings settings;
    settings.mode = Settings::stringToMode(this->ui->cbMode->currentText());

    settings.serial.portName = this->ui->leSerialPort->text();
    settings.serial.baudRate = static_cast<QSerialPort::BaudRate>(this->ui->cbSerialBaudRate->currentText().toInt());
    settings.serial.dataBits = static_cast<QSerialPort::DataBits>(this->ui->cbSerialDataBits->currentText().toInt());
    QString text = this->ui->cbSerialParity->currentText() + "Parity";
    int value = QMetaEnum::fromType<QSerialPort::Parity>().keyToValue(text.toUtf8().constData());
    settings.serial.parity = static_cast<QSerialPort::Parity>(value);
    float stopBits = this->ui->cbStopBits->currentText().toFloat();
    if (stopBits == 1.5f)
        settings.serial.stopBits = QSerialPort::StopBits::OneAndHalfStop;
    else
        settings.serial.stopBits = static_cast<QSerialPort::StopBits>(stopBits);

    settings.network.address = this->ui->leNetworkAddress->text();
    settings.network.port = this->ui->leNetworkPort->text().toUInt();

    settings.recordDestination = this->ui->leDestination->text();

    return settings;
}

void SettingsWindow::selectRecordDirectory() {
    QFileDialog dialog(this);

    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    QString selectedDirectory = dialog.getExistingDirectory(this, "Select Directory", QDir::homePath());
    if (!selectedDirectory.isEmpty()) {
        this->ui->leDestination->setText(selectedDirectory);
    }
}
