#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <cstdint>
#include <optional>

#include <QUdpSocket>

#include "BaseClient.h"
#include "Settings.h"

namespace CarQt {
class UDPClient : public BaseClient {
    Q_OBJECT

    uint32_t m_port;
    QUdpSocket m_socket;
    QHostAddress m_address;

  public:
    UDPClient(const struct Settings::Network &settings, QObject *parent);

    ~UDPClient() override = default;

    std::optional<Shared::Data> Data() override;

    [[nodiscard]] bool IsConnected() const override;

    void Connect() override;
};
} // namespace CarQt

#endif // UDPCLIENT_H
