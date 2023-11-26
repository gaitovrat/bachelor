#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>

#include "baseclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow *ui;
    std::unique_ptr<BaseClient> client;

public:
    MainWindow(QWidget *parent = nullptr);

    virtual ~MainWindow();
};
#endif // MAINWINDOW_H
