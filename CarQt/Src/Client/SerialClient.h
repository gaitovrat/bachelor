#ifndef SERIALCLIENT_H
#define SERIALCLIENT_H

#include <QSerialPort>

#include "BaseClient.h"
#include "Settings.h"

namespace CarQt {
class SerialClient : public BaseClient {
    Q_OBJECT

    QSerialPort port;

  public:
    SerialClient(const struct Settings::Serial &settings, QObject *parent);

    ~SerialClient() override = default;

    std::optional<Shared::Data> Data() override;

    [[nodiscard]] bool IsConnected() const override;

    void Connect() override;
};
} // namespace CarQt

#endif // SERIALCLIENT_H
