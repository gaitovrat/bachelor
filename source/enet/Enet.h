#ifndef ENET_ENET_H_
#define ENET_ENET_H_

#include "lwip/udp.h"
#include <cstdint>

static constexpr uint32_t BUF_N = 4;
static constexpr size_t BUF_LEN = 2048;

class Enet {
public:
	Enet();

	virtual ~Enet() = default;

	void Init(uint32_t data_len_limit, uint16_t port);

	void Send(const void *ptr_data, uint32_t len);

	bool Check();

private:
	bool m_initialized;

    struct netif m_netif;
    ip4_addr_t m_ipaddr, m_netmask, m_gateway, m_pc_ipaddr;

    pbuf *m_pbuffer[BUF_N];
    uint8_t m_data[BUF_N][BUF_LEN];

    udp_pcb *m_pcb;
    uint16_t m_port;
    uint32_t m_data_len;
    uint32_t m_data_i;
};
#endif /* ENET_ENET_H_ */
