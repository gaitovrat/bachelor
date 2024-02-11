#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <cstdint>
#include <optional>

#include <QUdpSocket>

#include "BaseClient.h"
#include "Settings.h"

class UDPClient : public BaseClient {
    Q_OBJECT

    uint32_t port;
    QUdpSocket socket;
    QHostAddress address;

   public:
    UDPClient(const struct Settings::Network& settings, QObject *parent);

    virtual ~UDPClient() = default;

    std::optional<Data> getData() override;

    bool connected() const override;

    void connect() override;
};

#endif  // UDPCLIENT_H
