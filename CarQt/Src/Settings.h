#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <optional>

#include <QSerialPort>
#include <QUdpSocket>

namespace CarQt {
struct Settings {
  public:
    enum Mode { Serial, Network };

    struct Serial {
        QString PortName;
        QSerialPort::BaudRate BaudRate;
        QSerialPort::DataBits DataBits;
        QSerialPort::Parity Parity;
        QSerialPort::StopBits StopBits;

        Serial();
    };

    struct Network {
        QHostAddress Address;
        uint32_t Port;

        Network();
    };

    Mode AppMode;
    struct Serial ClientSerial;
    struct Network ClientNetwork;
    QString RecordDestination;

    Settings();

    void Save(const char *filename) const;

    static std::optional<Settings> Load(const char *filename);

    static const char *ModeToString(Mode mode);

    static Mode ModeFromString(const QString &value);
};
} // namespace CarQt

#endif
