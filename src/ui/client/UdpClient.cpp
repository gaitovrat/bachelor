#include "UdpClient.h"

#include <cstring>
#include <optional>

#include "Data.h"

UDPClient::UDPClient(const QHostAddress& address, const int port,
                     QObject* parent)
    : port(port), socket(parent), address(address), BaseClient(parent) {
    this->connect(&this->socket, &QUdpSocket::readyRead);
    this->socket.bind(this->address, this->port, QUdpSocket::ShareAddress);
}

UDPClient::UDPClient(const QString& address, const int port, QObject* parent)
    : UDPClient(QHostAddress(address), port, parent) {}

UDPClient::UDPClient(const int port, QObject* parent)
    : UDPClient(QHostAddress::Any, port, parent) {}

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
