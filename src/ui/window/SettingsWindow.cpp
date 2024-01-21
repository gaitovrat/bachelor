#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

std::optional<std::string> SettingsWindow::execute() {
    int status = this->exec();
    if (status == 0) {
        return std::nullopt;
    }

    return "SettingsWindow";
}
