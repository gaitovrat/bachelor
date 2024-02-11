#include "SerialClient.h"

static constexpr int BUFFER_SIZE = 1024;

SerialClient::SerialClient(const struct Settings::Serial& settings, QObject *parent)
    : BaseClient(parent), port(settings.portName, parent) {
    this->bind(&this->port, &QSerialPort::readyRead);

    this->port.setBaudRate(settings.baudRate);
    this->port.setDataBits(settings.dataBits);
    this->port.setParity(settings.parity);
    this->port.setStopBits(settings.stopBits);

    this->connect();
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

bool SerialClient::connected() const {
    return this->port.isOpen();
}

void SerialClient::connect() {
    this->port.open(QSerialPort::ReadWrite);
}