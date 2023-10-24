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

static mdio_handle_t mdioHandle = {.ops = &enet_ops};
static phy_handle_t phyHandle   = {.phyAddr = BOARD_ENET0_PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &phyksz8081_ops};
static ethernetif_config_t config = {.phyHandle = &phyHandle, .macAddress = {0x02, 0x12, 0x13, 0x10, 0x15, 0x11}};

static constexpr u8_t configIP[4] = { 192, 168, 99, 101 };
static constexpr u8_t configNET[4] = { 255, 255, 255, 0 };
static constexpr u8_t configGW[4] = { 192, 168, 99, 100 };
static constexpr u8_t configPC[4] = {192, 168, 99, 255 };


static constexpr void MakeIp4Address(ip4_addr_t& address, const u8_t parts[4])
{
	IP4_ADDR(&address, parts[0], parts[1], parts[2], parts[3]);
}

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

Enet::Enet() : m_initialized(false) {}

void Enet::Init(const uint32_t data_len_limit, const uint16_t port) {
	BOARD_InitENETPins();

	mdioHandle.resource.csrClock_Hz = CLOCK_GetFreq(kCLOCK_CoreSysClk);

	MakeIp4Address(m_gateway, configGW);
	MakeIp4Address(m_ipaddr, configIP);
	MakeIp4Address(m_netmask, configNET);
	MakeIp4Address(m_pc_ipaddr, configPC);

	m_port = port;
	m_data_i = 0;
	m_data_len = MIN(BUF_LEN, data_len_limit);

	time_init();
	lwip_init();

	if (netif_add(&m_netif, &m_ipaddr, &m_netmask, &m_gateway,
			&config, ethernetif0_init, ethernet_input) == nullptr) {
		return;
	}
	netif_set_default(&m_netif);
	netif_set_up(&m_netif);

	PRINTF("\r\n************************************************\r\n");
	PRINTF(" NETIF Initialized\r\n");
	PRINTF("************************************************\r\n");
	PRINTF(" IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&m_ipaddr)[0], ((u8_t *)&m_ipaddr)[1],
		   ((u8_t *)&m_ipaddr)[2], ((u8_t *)&m_ipaddr)[3]);
	PRINTF(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&m_netmask)[0], ((u8_t *)&m_netmask)[1],
		   ((u8_t *)&m_netmask)[2], ((u8_t *)&m_netmask)[3]);
	PRINTF(" IPv4 Gateway     : %u.%u.%u.%u\r\n", ((u8_t *)&m_gateway)[0], ((u8_t *)&m_gateway)[1],
		   ((u8_t *)&m_gateway)[2], ((u8_t *)&m_gateway)[3]);
	PRINTF("************************************************\r\n");

	for (size_t i = 0; i < BUF_N; ++i) {
		m_pbuffer[i] = pbuf_alloc(PBUF_TRANSPORT, m_data_len, PBUF_REF);
		m_pbuffer[i]->payload = m_data[i];
	}

	PRINTF("\r\nUDP initialize...\r\n");
	m_pcb = udp_new();
	udp_bind(m_pcb, &m_ipaddr, m_port);
	udp_connect(m_pcb, &m_pc_ipaddr, m_port);

	m_initialized = true;
}

void Enet::Send(const void *ptr_data, const uint32_t len) {
	if (m_initialized == false) return;

	memcpy(m_data[m_data_i], ptr_data, MIN(m_data_len, len));
	udp_send(m_pcb, m_pbuffer[m_data_i]);

	m_data_i = (m_data_i + 1) % BUF_N;
}

bool Enet::Check()
{
	if (m_initialized == false) return false;

	ethernetif_input(&m_netif);  // Maybe some unexpected data
	sys_check_timeouts(); // Handle all system timeouts for all core protocols

	return true;
}
