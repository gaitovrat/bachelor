#include "udpclient.h"

UDPClient::UDPClient(const QHostAddress& address, const int port, QObject *parent)
    : port(port),
      socket(parent),
      address(address),
      BaseClient(&this->socket, &QUdpSocket::readyRead, parent) {
    this->socket.bind(this->address, this->port, QUdpSocket::ShareAddress);
}

UDPClient::UDPClient(const QString& address, const int port, QObject *parent) : UDPClient(QHostAddress(address), port, parent) {}

UDPClient::UDPClient(const int port, QObject* parent) : UDPClient(QHostAddress::Any, port, parent) {}

void UDPClient::emitData() {
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;

    buffer.resize(this->socket.pendingDatagramSize());
    this->socket.readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    qDebug() << buffer.size() << ' ' << buffer.data();
}
