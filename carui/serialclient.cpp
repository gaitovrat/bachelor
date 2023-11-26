#include "serialclient.h"

#include <QDebug>

static constexpr int BUFFER_SIZE = 1024;

SerialClient::SerialClient(const QSerialPort::BaudRate baudRate, QObject *parent) :
    port(parent),
    BaseClient(&this->port, &QSerialPort::readyRead, parent) {
    port.setBaudRate(baudRate);
}

SerialClient::SerialClient(QObject *parent) : SerialClient(QSerialPort::Baud115200, parent) {}

void SerialClient::emitData() {
    const QByteArray &buffer = this->port.readAll();

    qDebug() << buffer.size() << ' ' << buffer.data();
}
