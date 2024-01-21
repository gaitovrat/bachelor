#include "SettingsWindow.h"

#include <json/json.h>

#include <fstream>
#include <string>

#include "ui_SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsWindow) {
    ui->setupUi(this);
    this->load();

    this->ui->portNameLineEdit->setText(this->settings.portName);
}

SettingsWindow::~SettingsWindow() { delete ui; }

std::optional<Settings> SettingsWindow::execute() {
    int status = this->exec();
    if (status == 0) {
        return std::nullopt;
    }

    this->update();
    this->save();
    return this->settings;
}

void SettingsWindow::load() {
    std::ifstream fin(SettingsWindow::FILENAME);
    Json::Reader reader;
    Json::Value root;

    if (!fin.is_open()) {
        return;
    }

    if (!reader.parse(fin, root, false)) {
        qWarning() << "Unable to parse" << SettingsWindow::FILENAME;
        return;
    }

    if (root.isMember("portName")) {
        this->settings.portName = root["portName"].asCString();
    }
}

void SettingsWindow::save() {
    Json::Value root;
    Json::StreamWriterBuilder writer;
    std::ofstream fout(SettingsWindow::FILENAME);

    if (!fout.is_open()) {
        qWarning() << "Unable to open" << SettingsWindow::FILENAME;
        return;
    }

    root["portName"] = this->settings.portName.toUtf8().constData();

    fout << Json::writeString(writer, root);
}

void SettingsWindow::update() {
    this->settings.portName = this->ui->portNameLineEdit->text();
}