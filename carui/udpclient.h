#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>

class UDPClient : public QObject
{
    Q_OBJECT

public:
    UDPClient(const QHostAddress& address, int port, QObject *parent);

    UDPClient(const QString& address, int port, QObject *parent);

    UDPClient(int port, QObject *parent);

    virtual ~UDPClient() = default;

public slots:
    void Read();

private:
    uint32_t mPort;
    QUdpSocket mUdpSocket;
    QHostAddress mHostAddress;
};

#endif // UDPCLIENT_H
