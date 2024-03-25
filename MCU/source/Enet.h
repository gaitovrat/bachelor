#ifndef ENET_ENET_H_
#define ENET_ENET_H_

#include "Shared/Data.h"
#include "lwip/udp.h"
#include <cstdint>

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
    Enet();

    virtual ~Enet() = default;

    void init(const size_t bufferSize, const uint16_t port);

    void send(const void *pData, const uint32_t len);

    bool check();
};
} // namespace MCU
#endif /* ENET_ENET_H_ */
