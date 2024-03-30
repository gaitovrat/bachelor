#include "SensorsDialog.h"
#include "ui_SensorsDialog.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

using namespace CarQt;

SensorsDialog::SensorsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SensorsDialog) {
    ui->setupUi(this);
    this->setWindowTitle("Sensors");

    constexpr int numSensors = std::extent<decltype(this->sensors)>::value;
    for (int i = 0; i < numSensors; ++i) {
        this->sensors[i] = new SensorSeries;
        constexpr int numSeries =
            std::extent<decltype(this->sensors[0]->serieses)>::value;
        for (int j = 0; j < numSeries; ++j) {
            this->sensors[i]->serieses[j] = new QLineSeries(this);
        }
    }

    QChart *accelerometerChart =
        createChart("Accelerometer", *this->accelerometer);
    QChart *gyroscopeChart = createChart("Gyroscope", *this->gyroscope);
    QChart *magnetometerChart =
        createChart("Magnetometer", *this->magnetometer);
    QChart *charts[] = {accelerometerChart, gyroscopeChart, magnetometerChart};

    for (auto *chart : charts) {
        auto *chartView = new QChartView(chart, this);
        chartView->setRenderHint(QPainter::Antialiasing);
        this->ui->layout->addWidget(chartView);
    }
}

SensorsDialog::~SensorsDialog() {
    for (auto *sensor : this->sensors) {
        for (auto *series : sensor->serieses) {
            delete series;
        }
        delete sensor;
    }
    delete ui;
}

QChart *SensorsDialog::createChart(const QString &name,
                                   SensorSeries &sensorSeries) {
    auto *chart = new QChart();
    QString titles[] = {"X", "Y", "Z"};

    for (auto *series : sensorSeries.serieses) {
        chart->addSeries(series);
    }
    chart->legend()->hide();
    chart->setTitle(name);

    for (int i = 0; i < 3; ++i) {
        createAxises(*sensorSeries.serieses[i], titles[i]);
    }

    return chart;
}

void SensorsDialog::createAxises(QLineSeries &series, const QString &title) {
    auto *axisX = new QValueAxis(this);
    axisX->setTitleText("Time");
    series.attachAxis(axisX);

    auto *axisY = new QValueAxis(this);
    axisY->setTitleText(title);
    series.attachAxis(axisY);
}

void SensorsDialog::add(const Shared::SensorData &data, const uint64_t time) {
    QLineSeries *accelerometerSerieses[] = {this->accelerometer->serieses[0],
                                            this->accelerometer->serieses[1],
                                            this->accelerometer->serieses[2]};
    QLineSeries *gyroscopeSerieses[] = {this->gyroscope->serieses[0],
                                        this->gyroscope->serieses[1],
                                        this->gyroscope->serieses[2]};
    QLineSeries *magnetometerSerieses[] = {this->magnetometer->serieses[0],
                                           this->magnetometer->serieses[1],
                                           this->magnetometer->serieses[2]};

    int16_t accelValues[] = {data.accel.x, data.accel.y, data.accel.z};
    int16_t magValues[] = {data.mag.x, data.mag.y, data.mag.z};
    int16_t gyroValues[] = {data.gyro.x, data.gyro.y, data.gyro.z};

    for (int i = 0; i < 3; ++i) {
        accelerometerSerieses[i]->append(time, accelValues[i]);
        gyroscopeSerieses[i]->append(time, gyroValues[i]);
        magnetometerSerieses[i]->append(time, magValues[i]);
    }
}
