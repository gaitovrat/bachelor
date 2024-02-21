/*
 * Enet.cpp
 *
 *  Created on: Feb 12, 2023
 *      Author: Ratmir Gaitov
 */

#include "Enet.h"

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

static mdio_handle_t MDIO_HANDLE = {.ops = &enet_ops};
static phy_handle_t PHY_HANDLE   = {.phyAddr = BOARD_ENET0_PHY_ADDRESS, .mdioHandle = &MDIO_HANDLE, .ops = &phyksz8081_ops};
static ethernetif_config_t ETHERNETIF_CONFIG = {.phyHandle = &PHY_HANDLE, .macAddress = {0x02, 0x12, 0x13, 0x10, 0x15, 0x11}};

static constexpr u8_t MCU_IP[4] = { 192, 168, 99, 101 };
static constexpr u8_t NETMASK[4] = { 255, 255, 255, 0 };
static constexpr u8_t GATEWAY[4] = { 192, 168, 99, 100 };
static constexpr u8_t PC_IP[4] = {192, 168, 99, 255 };

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

Enet::Enet() : initialized(false) {}

void Enet::init(const uint32_t bufferSize, const uint16_t port) {
	/* Disable SYSMPU. */
	SYSMPU->CESR &= ~SYSMPU_CESR_VLD_MASK;
	/* Set RMII clock src. */
	SIM->SOPT2 |= SIM_SOPT2_RMIISRC_MASK;

	BOARD_InitENETPins();

	MDIO_HANDLE.resource.csrClock_Hz = CLOCK_GetFreq(kCLOCK_CoreSysClk);

	MakeIp4Address(this->gateway, GATEWAY);
	MakeIp4Address(this->mcuIp, MCU_IP);
	MakeIp4Address(this->netmask, NETMASK);
	MakeIp4Address(this->pcIp, PC_IP);

	this->port = port;
	this->dataIndex = 0;
	this->dataLen = MIN(Enet::BUFFER_SIZE, bufferSize);

	time_init();
	lwip_init();

	if (netif_add(&this->netif, &this->mcuIp, &this->netmask, &this->gateway,
			&ETHERNETIF_CONFIG, ethernetif0_init, ethernet_input) == nullptr) {
		return;
	}
	netif_set_default(&this->netif);
	netif_set_up(&this->netif);

	PRINTF("\r\n************************************************\r\n");
	PRINTF(" NETIF Initialized\r\n");
	PRINTF("************************************************\r\n");
	PRINTF(" IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&this->mcuIp)[0], ((u8_t *)&this->mcuIp)[1],
		   ((u8_t *)&this->mcuIp)[2], ((u8_t *)&this->mcuIp)[3]);
	PRINTF(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&this->netmask)[0], ((u8_t *)&this->netmask)[1],
		   ((u8_t *)&this->netmask)[2], ((u8_t *)&this->netmask)[3]);
	PRINTF(" IPv4 Gateway     : %u.%u.%u.%u\r\n", ((u8_t *)&this->gateway)[0], ((u8_t *)&this->gateway)[1],
		   ((u8_t *)&this->gateway)[2], ((u8_t *)&this->gateway)[3]);
	PRINTF("************************************************\r\n");

	for (size_t i = 0; i < Enet::BUFFER_COUNT; ++i) {
		this->pBuffer[i] = pbuf_alloc(PBUF_TRANSPORT, this->dataLen, PBUF_REF);
		this->pBuffer[i]->payload = this->data[i];
	}

	PRINTF("\r\nUDP initialize...\r\n");
	this->pcb = udp_new();
	udp_bind(this->pcb, &this->mcuIp, port);
	udp_connect(this->pcb, &this->pcIp, port);

	this->initialized = true;
}

void Enet::send(const void *pData, const uint32_t len) {
	if (this->initialized == false) return;

	memcpy(this->data[this->dataIndex], pData, MIN(this->dataLen, len));
	udp_send(this->pcb, this->pBuffer[this->dataIndex]);

	this->dataIndex = (this->dataIndex + 1) % Enet::BUFFER_COUNT;
}

bool Enet::check()
{
	if (this->initialized == false) return false;

	ethernetif_input(&this->netif);  // Maybe some unexpected data
	sys_check_timeouts(); // Handle all system timeouts for all core protocols

	return true;
}