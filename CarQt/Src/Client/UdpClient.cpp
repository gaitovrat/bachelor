#include "UdpClient.h"
#include <strings.h>

using namespace CarQt;

UDPClient::UDPClient(const struct Settings::Network &settings, QObject *parent)
    : m_port(settings.Port), m_socket(parent), m_address(settings.Address),
      BaseClient(parent) {
    Bind(&m_socket, &QUdpSocket::readyRead);
}

std::optional<Shared::Data> UDPClient::Data() {
    Shared::Data data;
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;

    bzero(&data, sizeof(data));

    qInfo() << sizeof(data) << sizeof(data.CarCameraData)
            << sizeof(data.CarMotorData) << sizeof(data.CarSteerData)
            << sizeof(data.CarSensorData) << sizeof(data.Timestamp);

    qint64 size = m_socket.pendingDatagramSize();
    emit DataReceived(size);
    qInfo() << size << sizeof(Shared::Data) << sizeof(Shared::SensorData);

    buffer.resize(size);

    m_socket.readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    std::memcpy(&data, buffer.data(), sizeof(Shared::Data));

    qInfo() << data.Timestamp;
    return data;
}

bool UDPClient::IsConnected() const {
    return m_socket.state() == QAbstractSocket::BoundState;
}

void UDPClient::Connect() {
    m_socket.bind(m_address, m_port, QUdpSocket::ShareAddress);
}
