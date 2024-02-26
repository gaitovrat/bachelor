#include "SerialClient.h"
#include <QtCore/qtypes.h>

using namespace CarQt;

static constexpr int BUFFER_SIZE = 1024;

SerialClient::SerialClient(const struct Settings::Serial &settings,
                           QObject *parent)
    : BaseClient(parent), port(settings.PortName, parent) {
    Bind(&port, &QSerialPort::readyRead);

    port.setBaudRate(settings.BaudRate);
    port.setDataBits(settings.DataBits);
    port.setParity(settings.Parity);
    port.setStopBits(settings.StopBits);
}

std::optional<Shared::Data> SerialClient::Data() {
    Shared::Data data;
    qint64 size = port.bytesAvailable();
    emit DataReceived(size);

    if (size < sizeof(Shared::Data)) {
        return std::nullopt;
    }

    const QByteArray &buffer = port.read(sizeof(Shared::Data));
    std::memcpy(&data, buffer.data(), sizeof(Shared::Data));

    return data;
}

bool SerialClient::IsConnected() const { return port.isOpen(); }

void SerialClient::Connect() { port.open(QSerialPort::ReadWrite); }
