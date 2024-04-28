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
    /*
     * Constructor
     * @param name the name of the window
     * @param parent the parent widget
     */
    explicit MainWindow(const QString &name, QWidget *parent = nullptr);

    /*
     * Destructor
     */
    ~MainWindow() override;

public slots:
    /*
     * Update the data on window
     * @param data the data to update with
     */
    void update(const Shared::Data &data);

    /*
     * Received size
     * @param size the size received
     */
    void receivedSize(qint64 size) const;

    /*
     * Transmit size
     * @param size the size transmitted
     */
    void transmitSize(qint64 size) const;

    /*
    * Open settings window
    */
    void openPreferences();

    /*
     * Reconnect to the server
     */
    void reconnect();

    /*
     * Record the data
     */
    void record();

private:
    /*
     * Update the client
     * @param settings the settings to update with
     */
    void updateClient(const Settings &settings);
};
}  // namespace CarQt

#endif  // MAINWINDOW_H
