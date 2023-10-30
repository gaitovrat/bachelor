#include "udpclient.h"

UDPClient::UDPClient(const QHostAddress& address, int port, QObject *parent)
    : QObject(parent),  mPort(port), mUdpSocket(parent), mHostAddress(address)
{
    mUdpSocket.bind(address, port, QUdpSocket::ShareAddress);
    connect(&mUdpSocket, &QUdpSocket::readyRead, this, &UDPClient::Read);
}

UDPClient::UDPClient(const QString& address, int port, QObject *parent) : UDPClient(QHostAddress(address), port, parent)
{}

UDPClient::UDPClient(int port, QObject* parent) : UDPClient(QHostAddress::Any, port, parent)
{}

void UDPClient::Read()
{
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;

    buffer.resize(mUdpSocket.pendingDatagramSize());
    mUdpSocket.readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    qDebug() << buffer.size() << ' ' << buffer.data();
}
