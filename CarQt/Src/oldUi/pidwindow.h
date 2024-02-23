#ifndef PIDWINDOW_H
#define PIDWINDOW_H

#include <QWidget>
#include <QLineSeries>
#include <QValueAxis>

#include <memory>
#include <list>

#include "./UDPClient.h"
#include "common/UdpData.h"

namespace Ui {
class PIDWindow;
}

class PIDWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PIDWindow(::std::shared_ptr<UDPClient> udp, QWidget *parent = nullptr);

    ~PIDWindow();

public slots:
    void DataReady(UdpData &udpData);

private:
    Ui::PIDWindow *ui;

    ::std::shared_ptr<UDPClient> m_udp;
    ::std::list<UdpData> m_historyData;

    QValueAxis* m_axises[4];
    QLineSeries* m_motorSeries[3];
    QLineSeries* m_steerSeries[3];
};

#endif // PIDWINDOW_H
