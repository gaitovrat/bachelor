#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/qlabel.h>
#include <memory>

#include "Client/BaseClient.h"
#include "Image.h"
#include "Recording.h"
#include "Settings.h"
#include <QLabel>
#include <QMainWindow>

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

  public:
    MainWindow(const QString &name, QWidget *parent = nullptr);

    virtual ~MainWindow();

  public slots:
    void update(const Shared::Data &data);

    void receivedSize(qint64 size);

    void openPreferences();

    void reconnect();

    void record();

  private:
    void updateClient(const Settings &settings);

    void updateConnected();

    Ui::MainWindow *m_ui;
    union {
        struct {
            QLabel *m_labelConnected;
            QLabel *m_labelTXBytes;
            QLabel *m_labelRXBytes;
        };
        QLabel *m_labels[3];
    };
    std::optional<Recording> m_recording;
    QString m_recordingPath;

    std::unique_ptr<BaseClient> m_client;
    std::list<Image> m_images;
};
} // namespace CarQt

#endif // MAINWINDOW_H
