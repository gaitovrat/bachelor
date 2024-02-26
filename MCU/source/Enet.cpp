#include "Enet.h"

#include <algorithm>
#include <string.h>

#include "enet_ethernetif.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"

#include "board.h"
#include "fsl_enet_mdio.h"
#include "fsl_phy.h"
#include "fsl_phyksz8081.h"
#include "pin_mux.h"

static mdio_handle_t MDIO_HANDLE = {.ops = &enet_ops};
static phy_handle_t PHY_HANDLE = {.phyAddr = BOARD_ENET0_PHY_ADDRESS,
                                  .mdioHandle = &MDIO_HANDLE,
                                  .ops = &phyksz8081_ops};
static ethernetif_config_t ETHERNETIF_CONFIG = {
    .phyHandle = &PHY_HANDLE,
    .macAddress = {0x02, 0x12, 0x13, 0x10, 0x15, 0x11}};

static constexpr u8_t MCU_IP[4] = {192, 168, 99, 101};
static constexpr u8_t NETMASK[4] = {255, 255, 255, 0};
static constexpr u8_t GATEWAY[4] = {192, 168, 99, 100};
static constexpr u8_t PC_IP[4] = {192, 168, 99, 255};

using namespace MCU;

#ifdef __cplusplus
extern "C" {
#endif
void SysTick_Handler(void) { time_isr(); }
#ifdef __cplusplus
}
#endif

static constexpr void MakeIp4Address(ip4_addr_t &address, const u8_t parts[4]) {
    IP4_ADDR(&address, parts[0], parts[1], parts[2], parts[3]);
}

Enet::Enet() : m_initialized(false) {}

void Enet::Init(const size_t bufferSize, const uint16_t port) {
    /* Disable SYSMPU. */
    SYSMPU->CESR &= ~SYSMPU_CESR_VLD_MASK;
    /* Set RMII clock src. */
    SIM->SOPT2 |= SIM_SOPT2_RMIISRC_MASK;

    BOARD_InitENETPins();

    MDIO_HANDLE.resource.csrClock_Hz = CLOCK_GetFreq(kCLOCK_CoreSysClk);

    MakeIp4Address(this->m_gateway, GATEWAY);
    MakeIp4Address(this->m_mcuIp, MCU_IP);
    MakeIp4Address(this->m_netmask, NETMASK);
    MakeIp4Address(this->m_pcIp, PC_IP);

    this->m_port = port;
    this->m_dataIndex = 0;
    this->m_dataLen = std::min(Enet::BUFFER_SIZE, bufferSize);

    time_init();
    lwip_init();

    if (netif_add(&this->m_netif, &this->m_mcuIp, &this->m_netmask,
                  &this->m_gateway, &ETHERNETIF_CONFIG, ethernetif0_init,
                  ethernet_input) == nullptr) {
        return;
    }
    netif_set_default(&this->m_netif);
    netif_set_up(&this->m_netif);

    PRINTF("\r\n************************************************\r\n");
    PRINTF(" NETIF Initialized\r\n");
    PRINTF("************************************************\r\n");
    PRINTF(" IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&this->m_mcuIp)[0],
           ((u8_t *)&this->m_mcuIp)[1], ((u8_t *)&this->m_mcuIp)[2],
           ((u8_t *)&this->m_mcuIp)[3]);
    PRINTF(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&this->m_netmask)[0],
           ((u8_t *)&this->m_netmask)[1], ((u8_t *)&this->m_netmask)[2],
           ((u8_t *)&this->m_netmask)[3]);
    PRINTF(" IPv4 Gateway     : %u.%u.%u.%u\r\n", ((u8_t *)&this->m_gateway)[0],
           ((u8_t *)&this->m_gateway)[1], ((u8_t *)&this->m_gateway)[2],
           ((u8_t *)&this->m_gateway)[3]);
    PRINTF("************************************************\r\n");

    for (size_t i = 0; i < Enet::BUFFER_COUNT; ++i) {
        this->m_pBuffer[i] =
            pbuf_alloc(PBUF_TRANSPORT, this->m_dataLen, PBUF_REF);
        this->m_pBuffer[i]->payload = this->m_data[i];
    }

    PRINTF("\r\nUDP initialize...\r\n");
    this->m_pcb = udp_new();
    udp_bind(this->m_pcb, &this->m_mcuIp, port);
    udp_connect(this->m_pcb, &this->m_pcIp, port);

    this->m_initialized = true;
}

void Enet::Send(const void *pData, const uint32_t len) {
    if (this->m_initialized == false)
        return;

    memcpy(this->m_data[this->m_dataIndex], pData,
           std::min(this->m_dataLen, len));
    udp_send(this->m_pcb, this->m_pBuffer[this->m_dataIndex]);

    this->m_dataIndex = (this->m_dataIndex + 1) % Enet::BUFFER_COUNT;
}

bool Enet::Check() {
    if (this->m_initialized == false)
        return false;

    ethernetif_input(&this->m_netif); // Maybe some unexpected data
    sys_check_timeouts(); // Handle all system timeouts for all core protocols

    return true;
}
