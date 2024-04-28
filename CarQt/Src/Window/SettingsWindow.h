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

    /*
     * Constructor
     * @param parent the parent widget
     */
    explicit SettingsWindow(QWidget *parent = nullptr);

    /*
     * Destructor
     */
    ~SettingsWindow() override;

    /*
     * Start the settings window
     * @return the settings
     */
    std::optional<Settings> execute();

public slots:
    /*
     * Select the record directory
     */
    void selectRecordDirectory();

private:
    /*
     * Get the settings
     */
    Settings getSettings() const;
};
}  // namespace CarQt

#endif  // SETTINGSWINDOW_H
