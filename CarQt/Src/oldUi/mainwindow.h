#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <SDL2/SDL.h>

#include <list>
#include <memory>

#include "UDPClient.h"
#include "./Image.h"
#include "./pidwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

public slots:
    void DataReady(UdpData &udpData);

    void OpenPID(bool);

    void HandleJoystick();

private:
    Ui::MainWindow *ui;

    ::std::shared_ptr<UDPClient> m_udp;
    ::std::list<::Client::Image> m_images;
    ::std::list<::UdpData> m_dataHistory;
    ::std::unique_ptr<PIDWindow> m_pidWindow;

    SDL_GameController *m_gamecontroller;
    QTimer m_timer;
};
#endif // MAINWINDOW_H
