#include "SerialClient.h"

#include <QtSerialPort/qserialport.h>

#include <QDebug>

#include "BaseClient.h"
#include "Data.h"

static constexpr int BUFFER_SIZE = 1024;

SerialClient::SerialClient(const QString &portName, QObject *parent)
    : SerialClient(portName, QSerialPort::BaudRate::Baud115200,
                   QSerialPort::DataBits::Data8, QSerialPort::Parity::NoParity,
                   QSerialPort::StopBits::OneStop, parent) {}

SerialClient::SerialClient(const QString &portName,
                           QSerialPort::BaudRate baudRate,
                           QSerialPort::DataBits dataBits,
                           QSerialPort::Parity parity,
                           QSerialPort::StopBits stopBits, QObject *parent)
    : BaseClient(parent), port(portName, parent) {
    this->connect(&this->port, &QSerialPort::readyRead);

    this->port.setBaudRate(baudRate);
    this->port.setDataBits(dataBits);
    this->port.setParity(parity);
    this->port.setStopBits(stopBits);

    if (this->port.open(QSerialPort::ReadWrite)) {
        qDebug() << "Connected to" << portName;
    } else {
        qDebug() << "Unnable to connect to" << portName;
    }
}

void SerialClient::emitData() {
    if (this->port.bytesAvailable() < sizeof(Data)) {
        return;
    }

    const QByteArray &buffer = this->port.read(sizeof(Data));
    const Data *data = (const Data *)buffer.data();
    qDebug() << data->accel.x << data->accel.y << data->accel.z;
}
