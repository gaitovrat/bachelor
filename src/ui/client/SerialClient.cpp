#include "SerialClient.h"

#include <QtSerialPort/qserialport.h>

#include <QDebug>
#include <cstring>
#include <optional>

#include "BaseClient.h"
#include "Data.h"

static constexpr int BUFFER_SIZE = 1024;

SerialClient::SerialClient(const struct Settings::Serial& settings, QObject *parent)
    : BaseClient(parent), port(settings.portName, parent) {
    this->connect(&this->port, &QSerialPort::readyRead);

    this->port.setBaudRate(settings.baudRate);
    this->port.setDataBits(settings.dataBits);
    this->port.setParity(settings.parity);
    this->port.setStopBits(settings.stopBits);

    if (this->port.open(QSerialPort::ReadWrite)) {
        qDebug() << "Connected to" << settings.portName;
    } else {
        qDebug() << "Unnable to connect to" << settings.portName;
    }
}

std::optional<Data> SerialClient::getData() {
    Data data;

    if (this->port.bytesAvailable() < sizeof(Data)) {
        return std::nullopt;
    }

    const QByteArray &buffer = this->port.read(sizeof(Data));
    std::memcpy(&data, buffer.data(), sizeof(Data));

    return data;
}
