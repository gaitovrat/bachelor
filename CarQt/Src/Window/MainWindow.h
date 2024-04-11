#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <memory>

#include "Image.h"
#include "Recording.h"
#include "Settings.h"
#include "UdpClient.h"

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

    QLabel *labelConnected;
    QLabel *labelTXBytes;
    QLabel *labelRXBytes;

    Ui::MainWindow *ui;
    Recording *recording;
    QString recordingPath;
    UDPClient *client;
    std::list<Image> images;

  public:
    explicit MainWindow(const QString &name, QWidget *parent = nullptr);

    ~MainWindow() override;

  public slots:
    void update(const Shared::Data &data);

    void receivedSize(qint64 size) const;

    void transmitSize(qint64 size) const;

    void openPreferences();

    void reconnect();

    void record();

  private:
    void updateClient(const Settings &settings);
};
} // namespace CarQt

#endif // MAINWINDOW_H
