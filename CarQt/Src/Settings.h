#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <QSerialPort>
#include <QUdpSocket>
#include <optional>

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

    /*
     * Constructor
     */
    Settings();

    /*
     * Save the settings to a file
     * @param filename the file to save to
     */
    void save(const char *filename) const;

    /*
     * Load the settings from a file
     * @param filename the file to load from
     * @return the settings
     */
    static Settings load(const char *filename);
};
}  // namespace CarQt

#endif
