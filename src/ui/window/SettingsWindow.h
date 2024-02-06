#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <optional>

#include <QDialog>

#include "Settings.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog {
    Q_OBJECT

    Ui::SettingsWindow *ui;

   public:
    static constexpr const char *FILENAME = "settings.json";

    explicit SettingsWindow(QWidget *parent = nullptr);

    ~SettingsWindow();

    std::optional<Settings> execute();

  public slots:
    void selectRecordDirectory();

   private:
    Settings getSettings();
};

#endif  // SETTINGSWINDOW_H
