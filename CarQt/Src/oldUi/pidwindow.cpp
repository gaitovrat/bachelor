#include "pidwindow.h"
#include "ui_pidwindow.h"

#include <QGraphicsScene>
#include <QChartView>
#include <QLineSeries>

static constexpr const char *names[3] = { "Input", "Set Point", "Output" };

PIDWindow::PIDWindow(::std::shared_ptr<UDPClient> udp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PIDWindow),
    m_udp(udp)
{
    ui->setupUi(this);

    for (int i = 0; i < 4; ++i) {
        m_axises[i] = new QValueAxis;
        m_axises[i]->setTitleText(i % 2 == 0 ? "Timestamps" : "Values");
    }

    QChart *motorChart = ui->ui_motorChartView->chart();
    motorChart->setTitle("Motor PID");
    motorChart->addAxis(m_axises[0], Qt::AlignBottom);
    motorChart->addAxis(m_axises[1], Qt::AlignLeft);

    QChart *steerChart = ui->ui_steerPIDView->chart();
    steerChart->setTitle("Steer PID");
    steerChart->addAxis(m_axises[2], Qt::AlignBottom);
    steerChart->addAxis(m_axises[3], Qt::AlignLeft);

    for (int i = 0; i < 3; ++i) {
        m_motorSeries[i] = new QLineSeries;
        m_steerSeries[i] = new QLineSeries;

        m_motorSeries[i]->setName(names[i]);
        m_steerSeries[i]->setName(names[i]);

        motorChart->addSeries(m_motorSeries[i]);
        steerChart->addSeries(m_steerSeries[i]);

        for (int j = 0; j < 2; ++j) {
            m_motorSeries[i]->attachAxis(m_axises[j]);
            m_steerSeries[i]->attachAxis(m_axises[2 + j]);
        }
    }

    connect(m_udp.get(), &UDPClient::dataReady, this, &PIDWindow::DataReady);
}

PIDWindow::~PIDWindow()
{
    for (const auto& el : m_axises) {
        delete el;
    }
    for (const auto& el : m_motorSeries) {
        delete el;
    }
    for (const auto& el : m_steerSeries) {
        delete el;
    }

    delete ui;
}

void PIDWindow::DataReady(UdpData &udpData) {
    m_historyData.push_back(udpData);

    double maxMotor = std::numeric_limits<double>::min();
    double minMotor = std::numeric_limits<double>::max();
    double maxSteer = std::numeric_limits<double>::min();
    double minSteer = std::numeric_limits<double>::max();

    ui->ui_pSteerLineEdit->setText(QString::number(udpData.SteerData.P));
    ui->ui_iSteerLineEdit->setText(QString::number(udpData.SteerData.I));
    ui->ui_dSteerLineEdit->setText(QString::number(udpData.SteerData.D));
    ui->ui_steerOutput->setText(QString::number(udpData.SteerData.Output));
    ui->ui_pMotorLineEdit->setText(QString::number(udpData.MotorData.P));
    ui->ui_iMotorLineEdit->setText(QString::number(udpData.MotorData.I));
    ui->ui_dMotorLineEdit->setText(QString::number(udpData.MotorData.D));
    ui->ui_motorOutput->setText(QString::number(udpData.MotorData.Output));

    for (const auto &el : m_historyData) {
        const PIDData &steerData = el.SteerData;
        const PIDData &motorData = el.MotorData;

        for (int i = 0; i < 3; ++i) {
            m_motorSeries[i]->clear();
            m_steerSeries[i]->clear();
        }

        m_motorSeries[0]->append(el.Timestamp, motorData.Input);
        m_motorSeries[1]->append(el.Timestamp, motorData.SetPoint);
        m_motorSeries[2]->append(el.Timestamp, motorData.Output);
        m_steerSeries[0]->append(el.Timestamp, steerData.Input);
        m_steerSeries[1]->append(el.Timestamp, steerData.SetPoint);
        m_steerSeries[2]->append(el.Timestamp, steerData.Output);

        double maxMotorEl = std::max({ motorData.Input, motorData.SetPoint, motorData.Output });
        double minMotorEl = std::min({ motorData.Input, motorData.SetPoint, motorData.Output });
        if (maxMotorEl > maxMotor) {
            maxMotor = maxMotorEl;
        }
        if (minMotorEl < minMotor) {
            minMotor = minMotorEl;
        }

        double maxSteerEl = std::max({ steerData.Input, steerData.SetPoint, steerData.Output });
        double minSteerEl = std::min({ steerData.Input, steerData.SetPoint, steerData.Output });
        if (maxSteerEl > maxSteer) {
            maxSteer = maxSteerEl;
        }
        if (minSteerEl < minSteer) {
            minSteer = minSteerEl;
        }
    }

    m_axises[0]->setRange(0, udpData.Timestamp);
    m_axises[2]->setRange(0, udpData.Timestamp);
    m_axises[1]->setRange(minMotor, maxMotor);
    m_axises[3]->setRange(minSteer, maxSteer);

    ui->ui_motorChartView->chart()->update();
    ui->ui_steerPIDView->chart()->update();
}
