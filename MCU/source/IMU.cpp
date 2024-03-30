/*
 * IMU.cpp
 *
 *  Created on: Mar 30, 2024
 *      Author: gaito
 */
#include "IMU.h"

#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_port.h"

#include "I2C.h"

static constexpr uint8_t CTRL_REG_1 = 0x2AU;
static constexpr uint8_t CTRL_REG_2 = 0x2BU;
static constexpr uint8_t CTRL_REG_3 = 0x2CU;
static constexpr uint8_t CTRL_REG_4 = 0x2DU;
static constexpr uint8_t CTRL_REG_5 = 0x2EU;
static constexpr uint8_t M_CTRL_REG_1 = 0x5BU;
static constexpr uint8_t M_CTRL_REG_2 = 0x5CU;
static constexpr uint8_t XYZ_DATA_CFG = 0x0EU;
static constexpr uint8_t STATUS = 0x00U;
static constexpr uint8_t M_STATUS = 0x32U;
static constexpr uint8_t F_SETUP = 0x09;
static constexpr uint32_t BAUDRATE = 100000;

using namespace MCU;

#if defined(__cplusplus)
extern "C" {
#endif
void PORTC_IRQHandler(void) {
    PRINTF("PORTC\r\n");

    PORT_ClearPinsInterruptFlags(PORTC, 1U << 17);
}
#if defined(__cplusplus)
}
#endif

IMU::IMU(FXOSRange range) : range(range) {}

status_t IMU::init() {
    status_t status;

    this->initMaster();
    status = this->initFXOS();

    PORT_SetPinInterruptConfig(PORTC, 17, kPORT_InterruptRisingEdge);
    PORT_ClearPinsInterruptFlags(PORTC, 1U << 17);

    status = this->startFXOS();
    NVIC_EnableIRQ(PORTC_IRQn);
    NVIC_SetPriority(PORTC_IRQn, 2);

    return status;
}

void IMU::initMaster() {
    i2c_master_config_t masterConfig;
    uint32_t sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);

    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = BAUDRATE;
    I2C_MasterInit(I2C0, &masterConfig, sourceClock);
    I2C_MasterTransferCreateHandle(I2C0, &handle, &callback, NULL);
}

status_t IMU::initFXOS() {
    status_t status = 0;
    uint8_t value = 0;

    // Set standby mode
    status = I2C::writeRegister(FXOS_ADDRESS, CTRL_REG_1, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Disable FIFO
    status = I2C::writeRegister(FXOS_ADDRESS, F_SETUP, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Enable auto-sleep, low power in sleep, high res in wake
    uint8_t mod = 0x2;
    uint8_t slpe = 0x4;
    uint8_t smod = 0x18;
    value = mod | slpe | smod;
    status = I2C::writeRegister(FXOS_ADDRESS, CTRL_REG_2, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Enable data ready interrupt
    uint8_t int_en_drdy = 1;
    status = I2C::writeRegister(FXOS_ADDRESS, CTRL_REG_4, &int_en_drdy);
    if (status != kStatus_Success) {
        return status;
    }

    // Interrupt is routed to INT1 pin
    uint8_t int_cfg_drdy = 1;
    status = I2C::writeRegister(FXOS_ADDRESS, CTRL_REG_5, &int_cfg_drdy);
    if (status != kStatus_Success) {
        return status;
    }

    // Set up Mag OSR and Hybrid mode
    uint8_t rst = 0x40U;
    uint8_t osr = 0x1CU;
    uint8_t hms = 0x03U;
    value = rst | osr | hms;
    status = I2C::writeRegister(FXOS_ADDRESS, M_CTRL_REG_1, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Enable hyrid mode auto increment
    value = 0x20U;
    status = I2C::writeRegister(FXOS_ADDRESS, M_CTRL_REG_2, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Set range
    value = range;
    status = I2C::writeRegister(FXOS_ADDRESS, XYZ_DATA_CFG, &value);
    if (status != kStatus_Success) {
        return status;
    }

    return status;
}

status_t IMU::startFXOS() {
    // Active, low noise, ODR 200 Hz
    uint8_t active = 0x01U;
    uint8_t dr = 0x08U;
    uint8_t lnoise = 0x04U;

    if (range == FXOSRange::G_8) {
        lnoise = 0;
    }

    uint8_t value = active | dr | lnoise;
    return I2C::writeRegister(FXOS_ADDRESS, CTRL_REG_1, &value);
}

void IMU::callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status,
                   void *userData) {}
