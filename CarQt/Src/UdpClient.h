#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QUdpSocket>
#include <cstdint>
#include <optional>

#include "Settings.h"
#include "Shared/Data.h"

namespace CarQt {
class UDPClient : public QObject {
    Q_OBJECT

    uint32_t port;
    QUdpSocket socket;
    QHostAddress address;
    QHostAddress targetAddress;

public:
    /*
     * Constructor
     * @param settings the network settings
     * @param parent the parent object
     */
    UDPClient(const struct Settings::Network &settings,
              QObject *parent = nullptr);

    /*
     * Destructor
    */
    virtual ~UDPClient() = default;

    /*
     * Get the data
     * @return the data
     */
    std::optional<Shared::Data> data();

    /*
     * Check if the client is connected
     * @return true if the client is connected, false otherwise
     */
    bool isConnected() const;

    /*
     * Bind the client
    */
    void bind();
public slots:
    /*
     * Read the data
     */
    void read();

signals:
    /*
     * Signal for when the data is ready
     * @param data the data
     */
    void dataReady(const Shared::Data &data);

    /*
     * Signal for when data is received
     * @param size the size of the data
     */
    void dataReceived(uint64_t size);

    /*
     * Signal for when data is transmitted
     * @param size the size of the data
     */
    void dataTransmitted(uint64_t size);
};
}  // namespace CarQt

#endif  // UDPCLIENT_H
