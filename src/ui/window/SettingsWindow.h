#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <optional>
#include <string>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog {
    Q_OBJECT

    Ui::SettingsWindow *ui;

public:
    explicit SettingsWindow(QWidget *parent = nullptr);

    ~SettingsWindow();

    std::optional<std::string> execute();
};

#endif // SETTINGSWINDOW_H
