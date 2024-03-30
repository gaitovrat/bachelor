#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <memory>

#include "GameController.h"
#include "Image.h"
#include "Recording.h"
#include "Settings.h"
#include "UdpClient.h"
#include "Window/SensorsDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace CarQt {
class MainWindow : public QMainWindow {
    Q_OBJECT

    static constexpr const char *LABEL_TX_FORMAT = "TX: %lldB";
    static constexpr const char *LABEL_RX_FORMAT = "RX: %lldB";

    union {
        struct {
            QLabel *labelConnected;
            QLabel *labelJoystickConnected;
            QLabel *labelTXBytes;
            QLabel *labelRXBytes;
        };
        QLabel *labels[4]{};
    };

    QTimer timer;
    GameController joystick;
    Ui::MainWindow *ui;
    Recording *recording;
    QString recordingPath;
    UDPClient *client;
    std::list<Image> images;
    SensorsDialog *sensorsDialog;

  public:
    explicit MainWindow(const QString &name, QWidget *parent = nullptr);

    ~MainWindow() override;

  public slots:
    void update(const Shared::Data &data);

    void receivedSize(qint64 size) const;

    void openPreferences();

    void reconnect();

    void record();

    void updateJoystick();

    void handleJoystick() const;

    void openSensorsDialog();

    void destroySensorsDialog();

    void startCar();

  private:
    void updateClient(const Settings &settings);
};
} // namespace CarQt

#endif // MAINWINDOW_H
