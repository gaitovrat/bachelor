#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "udpclient.h"

static constexpr int PORT = 8080;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), client(std::make_unique<UDPClient>(PORT, parent)) {
    this->ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete this->ui;
}
