#include "mainwindow.h"
#include "./ui_mainwindow.h"

static constexpr int scPort = 8080;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), mUdpClient(scPort, parent)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

