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
	bool mInitialized;

    struct netif mNetif;
    ip4_addr_t mMcuIp, mNetmask, mGateway, mPcIp;

    pbuf *mPbuffer[Enet::BUFFER_NUMBER];
    uint8_t mData[Enet::BUFFER_NUMBER][Enet::MAX_BUFFER_SIZE];

    udp_pcb *mPcb;
    uint16_t mPort;
    uint32_t mDataLen;
    uint32_t miData;
};
#endif /* ENET_ENET_H_ */
