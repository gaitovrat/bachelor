#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/qlabel.h>
#include <memory>

#include <QMainWindow>
#include <QLabel>
#include "client/BaseClient.h"
#include "Settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

    static constexpr const char *LABEL_BYTES_FORMAT = "%lldB";

    Ui::MainWindow* ui;
    union {
        struct {
            QLabel *labelConnected;
            QLabel *labelTXBytes;
            QLabel *labelRXBytes;
        };
        QLabel *labels[3];
    };

    std::unique_ptr<BaseClient> client;

   public:
    MainWindow(const QString& name, QWidget* parent = nullptr);

    virtual ~MainWindow();

   public slots:
    void update(const Data& data);

    void receivedSize(qint64 size);

    void openPreferences();

    void reconnect();

private:
    void updateClient(const Settings& settings);

    void updateConnected();
};
#endif  // MAINWINDOW_H
