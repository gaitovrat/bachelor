#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <QtCore/qstring.h>
#include <QSerialPort>

#include <QtSerialPort/qserialport.h>
#include <_types/_uint32_t.h>
#include <optional>


enum ModeSettings {
    Serial = 0,
    Netork
};

struct SerialSettings {
    QString portName;
    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;

    SerialSettings();
};

struct NetworkSettings {
    QString address;
    uint32_t port;

    NetworkSettings();
};

struct Settings {
    ModeSettings mode;
    SerialSettings serial;
    NetworkSettings network;
    QString recordDestination;

    Settings();

    void save(const char *filename);

    static std::optional<Settings> load(const char *filename);
};


#endif