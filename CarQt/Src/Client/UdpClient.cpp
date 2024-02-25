#include "UdpClient.h"

using namespace CarQt;

UDPClient::UDPClient(const struct Settings::Network& settings, QObject* parent)
    : m_port(settings.Port), m_socket(parent), m_address(settings.Address), BaseClient(parent) {
    Bind(&m_socket, &QUdpSocket::readyRead);
}

std::optional<Shared::Data> UDPClient::Data() {
    Shared::Data data;
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;

    qint64 size = m_socket.pendingDatagramSize();
    emit DataReceived(size);
    buffer.resize(size);

    m_socket.readDatagram(buffer.data(), buffer.size(), &sender,
                              &senderPort);

    std::memcpy(&data, buffer.data(), sizeof(Shared::Data));


    return data;
}

bool UDPClient::IsConnected() const {
    return m_socket.state() == QAbstractSocket::BoundState;
}

void UDPClient::Connect() {
    m_socket.bind(m_address, m_port, QUdpSocket::ShareAddress);
}
