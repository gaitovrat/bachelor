#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "udpclient.h"
#include "serialclient.h"

static constexpr int PORT = 8080;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
#ifdef USE_SERIAL
      client(std::make_unique<SerialClient>(parent))
#else
      client(std::make_unique<UDPClient>(PORT, parent))
#endif
{
    this->ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete this->ui;
}
