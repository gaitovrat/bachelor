#ifndef SERIALCLIENT_H
#define SERIALCLIENT_H

#include <QSerialPort>

#include "BaseClient.h"
#include "Settings.h"

class SerialClient : public BaseClient {
    Q_OBJECT

    QSerialPort port;

   public:
    SerialClient(const struct Settings::Serial& settings, QObject *parent);

    virtual ~SerialClient() = default;

    std::optional<Data> getData() override;

    bool connected() const override;

    void connect() override;
};

#endif  // SERIALCLIENT_H
