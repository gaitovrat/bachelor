#include "UdpClient.h"

UDPClient::UDPClient(const struct Settings::Network& settings, QObject* parent)
    : port(settings.port), socket(parent), address(settings.address), BaseClient(parent) {
    this->bind(&this->socket, &QUdpSocket::readyRead);
}

std::optional<Data> UDPClient::getData() {
    Data data;
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;

    buffer.resize(this->socket.pendingDatagramSize());
    this->socket.readDatagram(buffer.data(), buffer.size(), &sender,
                              &senderPort);

    std::memcpy(&data, buffer.data(), sizeof(Data));

    return data;
}

bool UDPClient::connected() const {
    return this->socket.state() == QAbstractSocket::BoundState;
}

void UDPClient::connect() {
    this->socket.bind(this->address, this->port, QUdpSocket::ShareAddress);
}
