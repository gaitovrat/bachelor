#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <cstdint>

#include <QUdpSocket>

#include "BaseClient.h"

class UDPClient : public BaseClient {
    uint32_t port;
    QUdpSocket socket;
    QHostAddress address;

public:
    UDPClient(const QHostAddress& address, int port, QObject *parent);

    UDPClient(const QString& address, int port, QObject *parent);

    UDPClient(int port, QObject *parent);

    virtual ~UDPClient() = default;

    void emitData() override;
};

#endif // UDPCLIENT_H
