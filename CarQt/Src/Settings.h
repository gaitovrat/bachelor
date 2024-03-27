#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <optional>

#include <QSerialPort>
#include <QUdpSocket>

namespace CarQt {
struct Settings {
  public:
    struct Network {
        QHostAddress pcAddress;
        QHostAddress mcuAddress;
        uint32_t port;

        Network();
    };

    Network network;
    QString recordDestination;

    Settings();

    void save(const char *filename) const;

    static Settings load(const char *filename);
};
} // namespace CarQt

#endif
