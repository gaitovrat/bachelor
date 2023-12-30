#ifndef SERIALCLIENT_H
#define SERIALCLIENT_H

#include <QSerialPort>

#include "BaseClient.h"

class SerialClient : public BaseClient {
    QSerialPort port;

public:
    SerialClient(QSerialPort::BaudRate baudRate, QObject *parent);

    SerialClient(QObject *parent);

    virtual ~SerialClient() = default;

    void emitData() override;
};

#endif // SERIALCLIENT_H
