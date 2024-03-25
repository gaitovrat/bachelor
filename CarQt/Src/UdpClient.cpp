#include "UdpClient.h"

using namespace CarQt;

UDPClient::UDPClient(const struct Settings::Network &settings, QObject *parent)
    : port(settings.port), socket(parent), address(settings.address) {
    connect(&this->socket, &QUdpSocket::readyRead, this, &UDPClient::read);
}

std::optional<Shared::Data> UDPClient::data() {
    Shared::Data data;
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;

    qint64 size = this->socket.pendingDatagramSize();
    emit dataReceived(size);

    if (size != sizeof(Shared::Data)) {
        return std::nullopt;
    }

    buffer.resize(size);

    this->socket.readDatagram(buffer.data(), buffer.size(), &sender,
                              &senderPort);

    std::memcpy(&data, buffer.data(), sizeof(Shared::Data));

    return data;
}

bool UDPClient::isConnected() const {
    return this->socket.state() == QAbstractSocket::BoundState;
}

void UDPClient::bind() {
    this->socket.bind(this->address, this->port, QUdpSocket::ShareAddress);
}

void UDPClient::read() {
    std::optional<Shared::Data> data = this->data();

    if (data.has_value()) {
        emit dataReady(*data);
    }
}
