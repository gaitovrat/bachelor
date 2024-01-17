#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

#include "client/BaseClient.h"
#include "client/SerialClient.h"

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
    MainWindow(QWidget* parent = nullptr);

    virtual ~MainWindow();

   public slots:
    void update(const Data& data);
};
#endif  // MAINWINDOW_H
