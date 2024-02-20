#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <optional>

#include <QSerialPort>
#include <QUdpSocket>

struct Settings {
    enum Mode {
        Serial = 0,
        Network
    };

    struct Serial {
        QString portName;
        QSerialPort::BaudRate baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;

        Serial();
    };

    struct Network {
        QHostAddress address;
        uint32_t port;

        Network();
    };

    Mode mode;
    struct Serial serial;
    struct Network network;
    QString recordDestination;

    Settings();

    void save(const char *filename);

    static std::optional<Settings> load(const char *filename);

    static const char *modeToString(Mode mode);
    static Mode stringToMode(const QString& value);
};


#endif
