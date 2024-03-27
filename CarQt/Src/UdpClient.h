#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <cstdint>
#include <optional>

#include <QUdpSocket>

#include "Settings.h"
#include "Shared/Data.h"
#include "Shared/Signal.h"

namespace CarQt {
class UDPClient : public QObject {
    Q_OBJECT

    uint32_t port;
    QUdpSocket socket;
    QHostAddress address;
    QHostAddress targetAddress;

  public:
    UDPClient(const struct Settings::Network &settings,
              QObject *parent = nullptr);

    virtual ~UDPClient() = default;

    std::optional<Shared::Data> data();

    bool isConnected() const;

    void bind();

    void send(const Shared::Signal &signal);
  public slots:
    void read();

  signals:
    void dataReady(const Shared::Data &data);

    void dataReceived(uint64_t size);
};
} // namespace CarQt

#endif // UDPCLIENT_H
