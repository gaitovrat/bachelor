#ifndef SERIALCLIENT_H
#define SERIALCLIENT_H

#include <QSerialPort>

#include "baseclient.h"

class SerialClient : public BaseClient {
    Q_OBJECT

    QSerialPort port;

public:
    SerialClient(QSerialPort::BaudRate baudRate, QObject *parent);

    SerialClient(QObject *parent);

    virtual ~SerialClient() = default;

    void emitData() override;
};

#endif // SERIALCLIENT_H
