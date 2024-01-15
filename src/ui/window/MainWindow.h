#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

#include "client/SerialClient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

    Ui::MainWindow* ui;
    SerialClient client;

   public:
    MainWindow(QWidget* parent = nullptr);

    virtual ~MainWindow();
};
#endif  // MAINWINDOW_H
