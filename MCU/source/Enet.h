#ifndef ENET_ENET_H_
#define ENET_ENET_H_

#include "Shared/Data.h"
#include "lwip/udp.h"
#include <cstdint>

namespace MCU {
class Enet {
    static constexpr uint32_t BUFFER_COUNT = 4;
    static constexpr size_t BUFFER_SIZE = 1024;

  public:
    Enet();

    virtual ~Enet() = default;

    void Init(const size_t bufferSize, const uint16_t port);

    void Send(const void *pData, const uint32_t len);

    bool Check();

  private:
    bool m_initialized;

    struct netif m_netif;
    ip4_addr_t m_mcuIp, m_netmask, m_gateway, m_pcIp;

    pbuf *m_pBuffer[Enet::BUFFER_COUNT];
    uint8_t m_data[Enet::BUFFER_COUNT][Enet::BUFFER_SIZE];

    udp_pcb *m_pcb;
    uint16_t m_port;
    uint32_t m_dataLen;
    uint32_t m_dataIndex;
};
} // namespace MCU
#endif /* ENET_ENET_H_ */
