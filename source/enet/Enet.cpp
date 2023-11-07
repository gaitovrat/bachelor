/*
 * Enet.cpp
 *
 *  Created on: Feb 12, 2023
 *      Author: Ratmir Gaitov
 */

#include "enet/Enet.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "enet_ethernetif.h"

#include "fsl_phy.h"
#include "fsl_phyksz8081.h"
#include "fsl_enet_mdio.h"
#include "board.h"
#include "pin_mux.h"

static mdio_handle_t sMdioHandle = {.ops = &enet_ops};
static phy_handle_t sPhyHandle   = {.phyAddr = BOARD_ENET0_PHY_ADDRESS, .mdioHandle = &sMdioHandle, .ops = &phyksz8081_ops};
static ethernetif_config_t sEtehernetifConfig = {.phyHandle = &sPhyHandle, .macAddress = {0x02, 0x12, 0x13, 0x10, 0x15, 0x11}};

static constexpr u8_t scMcuIp[4] = { 192, 168, 99, 101 };
static constexpr u8_t scNetmask[4] = { 255, 255, 255, 0 };
static constexpr u8_t scGateway[4] = { 192, 168, 99, 100 };
static constexpr u8_t scPcIp[4] = {192, 168, 99, 255 };

#ifdef __cplusplus
extern "C" {
#endif
void SysTick_Handler(void)
{
    time_isr();
}
#ifdef __cplusplus
}
#endif

static constexpr void MakeIp4Address(ip4_addr_t& address, const u8_t parts[4])
{
	IP4_ADDR(&address, parts[0], parts[1], parts[2], parts[3]);
}

Enet::Enet() : mInitialized(false) {}

void Enet::Init(const uint32_t bufferSize, const uint16_t port) {
	/* Disable SYSMPU. */
	SYSMPU->CESR &= ~SYSMPU_CESR_VLD_MASK;
	/* Set RMII clock src. */
	SIM->SOPT2 |= SIM_SOPT2_RMIISRC_MASK;

	BOARD_InitENETPins();

	sMdioHandle.resource.csrClock_Hz = CLOCK_GetFreq(kCLOCK_CoreSysClk);

	MakeIp4Address(mGateway, scGateway);
	MakeIp4Address(mMcuIp, scMcuIp);
	MakeIp4Address(mNetmask, scNetmask);
	MakeIp4Address(mPcIp, scPcIp);

	mPort = port;
	miData = 0;
	mDataLen = MIN(Enet::MAX_BUFFER_SIZE, bufferSize);

	time_init();
	lwip_init();

	if (netif_add(&mNetif, &mMcuIp, &mNetmask, &mGateway,
			&sEtehernetifConfig, ethernetif0_init, ethernet_input) == nullptr) {
		return;
	}
	netif_set_default(&mNetif);
	netif_set_up(&mNetif);

	PRINTF("\r\n************************************************\r\n");
	PRINTF(" NETIF Initialized\r\n");
	PRINTF("************************************************\r\n");
	PRINTF(" IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&mMcuIp)[0], ((u8_t *)&mMcuIp)[1],
		   ((u8_t *)&mMcuIp)[2], ((u8_t *)&mMcuIp)[3]);
	PRINTF(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&mNetmask)[0], ((u8_t *)&mNetmask)[1],
		   ((u8_t *)&mNetmask)[2], ((u8_t *)&mNetmask)[3]);
	PRINTF(" IPv4 Gateway     : %u.%u.%u.%u\r\n", ((u8_t *)&mGateway)[0], ((u8_t *)&mGateway)[1],
		   ((u8_t *)&mGateway)[2], ((u8_t *)&mGateway)[3]);
	PRINTF("************************************************\r\n");

	for (size_t i = 0; i < Enet::BUFFER_NUMBER; ++i) {
		mPbuffer[i] = pbuf_alloc(PBUF_TRANSPORT, mDataLen, PBUF_REF);
		mPbuffer[i]->payload = mData[i];
	}

	PRINTF("\r\nUDP initialize...\r\n");
	mPcb = udp_new();
	udp_bind(mPcb, &mMcuIp, mPort);
	udp_connect(mPcb, &mPcIp, mPort);

	mInitialized = true;
}

void Enet::Send(const void *pData, const uint32_t len) {
	if (mInitialized == false) return;

	memcpy(mData[miData], pData, MIN(mDataLen, len));
	udp_send(mPcb, mPbuffer[miData]);

	miData = (miData + 1) % Enet::BUFFER_NUMBER;
}

bool Enet::Check()
{
	if (mInitialized == false) return false;

	ethernetif_input(&mNetif);  // Maybe some unexpected data
	sys_check_timeouts(); // Handle all system timeouts for all core protocols

	return true;
}
