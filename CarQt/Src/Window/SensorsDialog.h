#ifndef SENSORSDIALOG_H
#define SENSORSDIALOG_H

#include "Shared/SensorData.h"
#include <QDialog>
#include <QtCharts/QLineSeries>
#include <vector>

namespace Ui {
class SensorsDialog;
}

namespace CarQt {
class SensorsDialog : public QDialog {
    Q_OBJECT

    union SensorSeries {
        struct {
            QLineSeries *xSeries;
            QLineSeries *ySeries;
            QLineSeries *zSeries;
        };
        QLineSeries *serieses[3]{};
    };
    union {
        struct {
            SensorSeries *accelerometer;
            SensorSeries *gyroscope;
            SensorSeries *magnetometer;
        };
        SensorSeries *sensors[3]{};
    };
    Ui::SensorsDialog *ui;

  public:
    explicit SensorsDialog(QWidget *parent = nullptr);

    ~SensorsDialog() override;

    void add(const Shared::SensorData &data, const uint64_t time);

  private:
    QChart *createChart(const QString &name, SensorSeries &sensorSeries);

    void createAxises(QLineSeries &series, const QString &title);
};
} // namespace CarQt

#endif // SENSORSDIALOG_H
