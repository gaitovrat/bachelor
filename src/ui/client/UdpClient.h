#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QtCore/qtmetamacros.h>

#include <QUdpSocket>
#include <cstdint>
#include <optional>

#include "BaseClient.h"

class UDPClient : public BaseClient {
    Q_OBJECT

    uint32_t port;
    QUdpSocket socket;
    QHostAddress address;

   public:
    UDPClient(const QHostAddress &address, int port, QObject *parent);

    UDPClient(const QString &address, int port, QObject *parent);

    UDPClient(int port, QObject *parent);

    virtual ~UDPClient() = default;

    std::optional<Data> getData() override;
};

#endif  // UDPCLIENT_H
