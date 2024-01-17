#ifndef SERIALCLIENT_H
#define SERIALCLIENT_H

#include <QtSerialPort/qserialport.h>

#include <QSerialPort>

#include "BaseClient.h"

class SerialClient : public BaseClient {
    Q_OBJECT

    QSerialPort port;

   public:
    SerialClient(const QString &portName, QObject *parent);

    SerialClient(const QString &portName, QSerialPort::BaudRate baudRate,
                 QSerialPort::DataBits dataBits, QSerialPort::Parity parity,
                 QSerialPort::StopBits stopBits, QObject *parent);

    virtual ~SerialClient() = default;

    std::optional<Data> getData() override;
};

#endif  // SERIALCLIENT_H
