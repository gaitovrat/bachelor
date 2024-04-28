#ifndef ENET_ENET_H_
#define ENET_ENET_H_

#include <cstdint>

#include "Shared/Data.h"
#include "lwip/udp.h"

namespace MCU {
class Enet {
    static constexpr uint32_t BUFFER_COUNT = 4;
    static constexpr size_t BUFFER_SIZE = 1024;

    bool initialized;

    struct netif netif;
    ip4_addr_t mcuIp, netmask, gateway, pcIp;

    pbuf *pBuffer[Enet::BUFFER_COUNT];
    uint8_t data[Enet::BUFFER_COUNT][Enet::BUFFER_SIZE];

    udp_pcb *pcb;
    uint16_t port;
    uint32_t dataLen;
    uint32_t dataIndex;

public:
    /*
     * Constructor
     */
    Enet();

    /*
     * Destructor
     */
    virtual ~Enet() = default;

    /*
     * Initialize the ENET
     * @param bufferSize the size of the buffer
     * @param port the port number
     */
    void init(const size_t bufferSize, const uint16_t port);

    /*
     * Send data
     * @param pData the data to send
     * @param len the length of the data
     */
    void send(const void *pData, const uint32_t len);

    /*
    * Check timeouts and initialization
    * @return true if the ENET is initialized, false otherwise
    */
    bool check();
};
}  // namespace MCU
#endif /* ENET_ENET_H_ */
