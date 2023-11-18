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

static mdio_handle_t s_MdioHandle = {.ops = &enet_ops};
static phy_handle_t s_PhyHandle   = {.phyAddr = BOARD_ENET0_PHY_ADDRESS, .mdioHandle = &s_MdioHandle, .ops = &phyksz8081_ops};
static ethernetif_config_t s_EtehernetifConfig = {.phyHandle = &s_PhyHandle, .macAddress = {0x02, 0x12, 0x13, 0x10, 0x15, 0x11}};

static constexpr u8_t sc_McuIp[4] = { 192, 168, 99, 101 };
static constexpr u8_t sc_Netmask[4] = { 255, 255, 255, 0 };
static constexpr u8_t sc_Gateway[4] = { 192, 168, 99, 100 };
static constexpr u8_t sc_PcIp[4] = {192, 168, 99, 255 };

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

Enet::Enet() : m_Initialized(false) {}

void Enet::Init(const uint32_t bufferSize, const uint16_t port) {
	/* Disable SYSMPU. */
	SYSMPU->CESR &= ~SYSMPU_CESR_VLD_MASK;
	/* Set RMII clock src. */
	SIM->SOPT2 |= SIM_SOPT2_RMIISRC_MASK;

	BOARD_InitENETPins();

	s_MdioHandle.resource.csrClock_Hz = CLOCK_GetFreq(kCLOCK_CoreSysClk);

	MakeIp4Address(m_Gateway, sc_Gateway);
	MakeIp4Address(m_McuIp, sc_McuIp);
	MakeIp4Address(m_Netmask, sc_Netmask);
	MakeIp4Address(m_PcIp, sc_PcIp);

	m_Port = port;
	mi_Data = 0;
	m_DataLen = MIN(Enet::MAX_BUFFER_SIZE, bufferSize);

	time_init();
	lwip_init();

	if (netif_add(&m_Netif, &m_McuIp, &m_Netmask, &m_Gateway,
			&s_EtehernetifConfig, ethernetif0_init, ethernet_input) == nullptr) {
		return;
	}
	netif_set_default(&m_Netif);
	netif_set_up(&m_Netif);

	PRINTF("\r\n************************************************\r\n");
	PRINTF(" NETIF Initialized\r\n");
	PRINTF("************************************************\r\n");
	PRINTF(" IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&m_McuIp)[0], ((u8_t *)&m_McuIp)[1],
		   ((u8_t *)&m_McuIp)[2], ((u8_t *)&m_McuIp)[3]);
	PRINTF(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&m_Netmask)[0], ((u8_t *)&m_Netmask)[1],
		   ((u8_t *)&m_Netmask)[2], ((u8_t *)&m_Netmask)[3]);
	PRINTF(" IPv4 Gateway     : %u.%u.%u.%u\r\n", ((u8_t *)&m_Gateway)[0], ((u8_t *)&m_Gateway)[1],
		   ((u8_t *)&m_Gateway)[2], ((u8_t *)&m_Gateway)[3]);
	PRINTF("************************************************\r\n");

	for (size_t i = 0; i < Enet::BUFFER_NUMBER; ++i) {
		m_Pbuffer[i] = pbuf_alloc(PBUF_TRANSPORT, m_DataLen, PBUF_REF);
		m_Pbuffer[i]->payload = m_Data[i];
	}

	PRINTF("\r\nUDP initialize...\r\n");
	m_Pcb = udp_new();
	udp_bind(m_Pcb, &m_McuIp, m_Port);
	udp_connect(m_Pcb, &m_PcIp, m_Port);

	m_Initialized = true;
}

void Enet::Send(const void *pData, const uint32_t len) {
	if (m_Initialized == false) return;

	memcpy(m_Data[mi_Data], pData, MIN(m_DataLen, len));
	udp_send(m_Pcb, m_Pbuffer[mi_Data]);

	mi_Data = (mi_Data + 1) % Enet::BUFFER_NUMBER;
}

bool Enet::Check()
{
	if (m_Initialized == false) return false;

	ethernetif_input(&m_Netif);  // Maybe some unexpected data
	sys_check_timeouts(); // Handle all system timeouts for all core protocols

	return true;
}
