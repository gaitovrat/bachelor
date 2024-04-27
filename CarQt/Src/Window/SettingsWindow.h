#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

#include "Settings.h"

namespace Ui {
class SettingsWindow;
}

namespace CarQt {
class SettingsWindow : public QDialog {
    Q_OBJECT

    Ui::SettingsWindow *ui;

public:
    static constexpr const char *FILENAME = "settings.json";

    explicit SettingsWindow(QWidget *parent = nullptr);

    ~SettingsWindow() override;

    std::optional<Settings> execute();

public slots:
    void selectRecordDirectory();

private:
    Settings getSettings() const;
};
}  // namespace CarQt

#endif  // SETTINGSWINDOW_H
