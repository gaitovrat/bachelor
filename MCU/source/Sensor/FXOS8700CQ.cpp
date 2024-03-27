/*
 * FXOS8700CQ.cpp
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#include "FXOS8700CQ.h"

#include <pin_mux.h>

#include "fsl_debug_console.h"
#include "fsl_port.h"

static constexpr uint8_t CTRL_REG_1 = 0x2AU;
static constexpr uint8_t CTRL_REG_2 = 0x2BU;
static constexpr uint8_t M_CTRL_REG_1 = 0x5BU;
static constexpr uint8_t M_CTRL_REG_2 = 0x5CU;
static constexpr uint8_t XYZ_DATA_CFG = 0x0EU;
static constexpr uint8_t STATUS = 0x00U;
static constexpr uint8_t M_STATUS = 0x32U;
static constexpr uint8_t F_SETUP = 0x09;

using namespace MCU;

#ifdef __cplusplus
extern "C" {
#endif
void PORTC_IRQHandler(void) {
    PRINTF("PORTC\r\n");

    PORT_ClearPinsInterruptFlags(PORTC, PORT_GetPinsInterruptFlags(PORTC));
}
#ifdef __cplusplus
}
#endif

FXOS8700CQ::FXOS8700CQ(FXOS8700CQ::Range range)
    : BaseSensor(I2C0), address(0x1DU), range(range) {}

status_t FXOS8700CQ::init() {
    status_t status = 0;

    BOARD_InitACCEL_I2CPins();
    status = BaseSensor::init();

    PORT_ClearPinsInterruptFlags(PORTC, PORT_GetPinsInterruptFlags(PORTC));
    NVIC_EnableIRQ(PORTC_IRQn);

    return status;
}

status_t FXOS8700CQ::setupRegisters() {
    status_t status = 0;
    uint8_t value = 0;
    uint8_t active = 0x01U;

    // Set standby mode
    status = readRegister(CTRL_REG_1, &value, sizeof(value));
    if (status != kStatus_Success) {
        return status;
    }
    value &= ~active;
    status = writeRegister(CTRL_REG_1, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Disable FIFO
    value = 0;
    status = writeRegister(F_SETUP, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Enable auto-sleep, low power in sleep, high res in wake
    uint8_t mod = 0x2;
    uint8_t slpe = 0x4;
    uint8_t smod = 0x18;
    value = mod | slpe | smod;
    status = writeRegister(CTRL_REG_2, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Set up Mag OSR and Hybrid mode
    uint8_t rst = 0x40U;
    uint8_t osr = 0x1CU;
    uint8_t hms = 0x03U;
    value = rst | osr | hms;
    status = writeRegister(M_CTRL_REG_1, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Enable hyrid mode auto increment
    value = 0x20U;
    status = writeRegister(M_CTRL_REG_2, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Set range
    value = range;
    status = writeRegister(XYZ_DATA_CFG, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Enable sampling
    uint8_t dr = 0x08U;
    uint8_t lnoise = 0x04U;
    if (range == Range::G_8) {
        lnoise = 0;
    }
    value = active | dr | lnoise;
    status = writeRegister(CTRL_REG_1, &value);
    if (status != kStatus_Success) {
        return status;
    }

    return status;
}

std::optional<FXOS8700CQ::Data> FXOS8700CQ::read() const {
    FXOS8700CQ::Data data;
    uint8_t buffer[7] = {0};
    status_t status = 0;

    status = readRegister(STATUS, buffer, sizeof(buffer));
    if (status != kStatus_Success) {
        return std::nullopt;
    }

    for (size_t i = 1; i < sizeof(buffer); i += 2) {
        size_t index = i / 2;
        data.accel.values[index] =
            ((int16_t)(((buffer[i] << 8U) | buffer[i + 1]))) >> 2U;
    }

    status = readRegister(M_STATUS, buffer, sizeof(buffer));
    if (status != kStatus_Success) {
        return std::nullopt;
    }
    for (size_t i = 1; i < sizeof(buffer); i += 2) {
        size_t index = i / 2;
        data.mag.values[index] =
            ((int16_t)(((buffer[i] << 8U) | buffer[i + 1])));
    }

    return data;
}

uint8_t FXOS8700CQ::deviceAddress() const { return this->address; }

uint8_t FXOS8700CQ::getDataRegister() const { return STATUS + 1; }

uint8_t *FXOS8700CQ::getData() {
    return reinterpret_cast<uint8_t *>(&this->data);
}

uint32_t FXOS8700CQ::getDataSize() const { return sizeof(FXOS8700CQ::Data); }
