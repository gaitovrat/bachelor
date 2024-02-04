#include "SettingsWindow.h"

#include <json/json.h>

#include <fstream>
#include <string>

#include "Settings.h"
#include "ui_SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsWindow) {
    ui->setupUi(this);
    std::optional<Settings> settings = Settings::load(SettingsWindow::FILENAME);

    if (!settings.has_value()) {
        return;
    }

    // this->ui->portNameLineEdit->setText(settings->portName);
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
    // settings.portName = this->ui->portNameLineEdit->text();

    return settings;
}
