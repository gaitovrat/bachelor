#ifndef ENET_ENET_H_
#define ENET_ENET_H_

#include "lwip/udp.h"
#include <cstdint>

class Enet {
private:
	static constexpr uint32_t BUFFER_NUMBER = 4;

	static constexpr size_t MAX_BUFFER_SIZE = 2048;

public:
	Enet();

	virtual ~Enet() = default;

	void Init(uint32_t bufferSize, uint16_t port);

	void Send(const void *pData, uint32_t len);

	bool Check();

private:
	bool m_Initialized;

    struct netif m_Netif;
    ip4_addr_t m_McuIp, m_Netmask, m_Gateway, m_PcIp;

    pbuf *m_Pbuffer[Enet::BUFFER_NUMBER];
    uint8_t m_Data[Enet::BUFFER_NUMBER][Enet::MAX_BUFFER_SIZE];

    udp_pcb *m_Pcb;
    uint16_t m_Port;
    uint32_t m_DataLen;
    uint32_t mi_Data;
};
#endif /* ENET_ENET_H_ */
