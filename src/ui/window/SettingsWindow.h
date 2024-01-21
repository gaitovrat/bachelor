#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <optional>

#include "Settings.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog {
    Q_OBJECT

    static constexpr const char *FILENAME = "settings.json";

    Ui::SettingsWindow *ui;
    Settings settings;

   public:
    explicit SettingsWindow(QWidget *parent = nullptr);

    ~SettingsWindow();

    std::optional<Settings> execute();

   private:
    void load();

    void save();

    void update();
};

#endif  // SETTINGSWINDOW_H
