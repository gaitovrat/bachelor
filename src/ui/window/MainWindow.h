#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>

#include "client/BaseClient.h"
#include "Settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

    Ui::MainWindow* ui;
    std::unique_ptr<BaseClient> client;

   public:
    MainWindow(const QString& name, QWidget* parent = nullptr);

    virtual ~MainWindow();

   public slots:
    void update(const Data& data);

    void openPreferences();

    void reconnect();

private:
    void updateClient(const Settings& settings);

    void updateConnected();
};
#endif  // MAINWINDOW_H
