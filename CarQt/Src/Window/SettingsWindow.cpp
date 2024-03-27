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
    Settings settings = Settings::load(SettingsWindow::FILENAME);

    ui->lePCNetworkAddress->setText(settings.network.pcAddress.toString());
    ui->leMCUNetworkAddress->setText(settings.network.mcuAddress.toString());
    ui->leNetworkPort->setText(QString::number(settings.network.port));

    ui->leDestination->setText(settings.recordDestination);

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
    settings.save(SettingsWindow::FILENAME);

    return settings;
}

Settings SettingsWindow::getSettings() const {
    Settings settings;

    settings.network.pcAddress = QHostAddress(ui->lePCNetworkAddress->text());
    settings.network.mcuAddress = QHostAddress(ui->leMCUNetworkAddress->text());
    settings.network.port = ui->leNetworkPort->text().toUInt();

    settings.recordDestination = ui->leDestination->text();

    return settings;
}

void SettingsWindow::selectRecordDirectory() {
    QString selectedDirectory = QFileDialog::getExistingDirectory(
        this, "Select Directory", QDir::homePath());
    if (!selectedDirectory.isEmpty()) {
        ui->leDestination->setText(selectedDirectory);
    }
}
