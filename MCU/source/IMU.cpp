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

#include "Core.h"
#include "I2C.h"

static constexpr uint8_t FXOS_CTRL_REG_1 = 0x2AU;
static constexpr uint8_t FXOS_CTRL_REG_2 = 0x2BU;
static constexpr uint8_t FXOS_CTRL_REG_3 = 0x2CU;
static constexpr uint8_t FXOS_CTRL_REG_4 = 0x2DU;
static constexpr uint8_t FXOS_CTRL_REG_5 = 0x2EU;
static constexpr uint8_t FXOS_M_CTRL_REG_1 = 0x5BU;
static constexpr uint8_t FXOS_M_CTRL_REG_2 = 0x5CU;
static constexpr uint8_t FXOS_XYZ_DATA_CFG = 0x0EU;
static constexpr uint8_t FXOS_OUT_X_MSB = 0x01U;
static constexpr uint8_t FXOS_M_STATUS = 0x32U;
static constexpr uint8_t FXOS_F_SETUP = 0x09;
static constexpr uint8_t FXAS_CTRL_REG0 = 0x0DU;
static constexpr uint8_t FXAS_CTRL_REG1 = 0x13U;
static constexpr uint8_t FXAS_CTRL_REG2 = 0x14U;
static constexpr uint8_t FXAS_CTRL_REG3 = 0x15U;
static constexpr uint8_t FXAS_F_SETUP = 0x09U;
static constexpr uint8_t FXAS_OUT_X_MSB = 0x01U;
static constexpr uint32_t BAUDRATE = 100000;

using namespace MCU;

extern Core core;

#if defined(__cplusplus)
extern "C" {
#endif
void PORTC_IRQHandler(void) {
    i2c_master_transfer_t masterXfer;
    IMU &imu = core.getIMU();

    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress = IMU::FXOS_ADDRESS;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress = FXOS_OUT_X_MSB;
    masterXfer.subaddressSize = 1;
    masterXfer.data = imu.getFXOSBuffer();
    masterXfer.dataSize = imu.getFXOSBufferSize();
    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferNonBlocking(I2C0, &imu.getHandle(), &masterXfer);

    PORT_ClearPinsInterruptFlags(PORTC, 1U << 13);
}

void PORTA_IRQHandler(void) {
    i2c_master_transfer_t masterXfer;
    IMU &imu = core.getIMU();

    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress = IMU::FXAS_ADDRESS;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress = FXAS_OUT_X_MSB;
    masterXfer.subaddressSize = 1;
    masterXfer.data = imu.getFXASBuffer();
    masterXfer.dataSize = imu.getFXASBufferSize();
    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferNonBlocking(I2C0, &imu.getHandle(), &masterXfer);

    PORT_ClearPinsInterruptFlags(PORTA, 1U << 29);
}
#if defined(__cplusplus)
}
#endif

IMU::IMU(FXOSRange fxosRange, FXASRange fxasRange)
    : fxosRange(fxosRange), fxasRange(fxasRange) {
    memset(this->fxosBuffer, 0, sizeof(this->fxosBuffer));
    memset(this->fxasBuffer, 0, sizeof(this->fxasBuffer));
}

status_t IMU::init() {
    status_t status;

    this->initMaster();
    status = this->initFXOS();
    status = this->initFXAS();

    PORT_SetPinInterruptConfig(PORTC, 13, kPORT_InterruptLogicZero);
    PORT_SetPinInterruptConfig(PORTA, 29, kPORT_InterruptLogicZero);
    PORT_ClearPinsInterruptFlags(PORTC, 1U << 13);
    PORT_ClearPinsInterruptFlags(PORTA, 1U << 29);

    status = this->startFXOS();
    status = this->startFXAS();

    NVIC_EnableIRQ(PORTC_IRQn);
    NVIC_EnableIRQ(PORTA_IRQn);
    NVIC_SetPriority(PORTC_IRQn, 0);
    NVIC_SetPriority(PORTA_IRQn, 0);

    return status;
}

void IMU::initMaster() {
    i2c_master_config_t masterConfig;
    uint32_t sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);

    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = BAUDRATE;
    I2C_MasterInit(I2C0, &masterConfig, sourceClock);
    I2C_MasterTransferCreateHandle(I2C0, &handle, &callback, this);
}

status_t IMU::initFXOS() {
    status_t status = 0;
    uint8_t value = 0;

    // Set standby mode
    status = I2C::writeRegister(FXOS_ADDRESS, FXOS_CTRL_REG_1, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Disable FIFO
    status = I2C::writeRegister(FXOS_ADDRESS, FXOS_F_SETUP, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Enable auto-sleep, low power in sleep, high res in wake
    uint8_t mod = 0x2;
    uint8_t slpe = 0x4;
    uint8_t smod = 0x18;
    value = mod | slpe | smod;
    status = I2C::writeRegister(FXOS_ADDRESS, FXOS_CTRL_REG_2, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Enable data ready interrupt
    uint8_t int_en_drdy = 1;
    status = I2C::writeRegister(FXOS_ADDRESS, FXOS_CTRL_REG_4, &int_en_drdy);
    if (status != kStatus_Success) {
        return status;
    }

    // Interrupt is routed to INT2 pin
    uint8_t int_cfg_drdy = 0;
    status = I2C::writeRegister(FXOS_ADDRESS, FXOS_CTRL_REG_5, &int_cfg_drdy);
    if (status != kStatus_Success) {
        return status;
    }

    // Set up Mag OSR and Hybrid mode
    uint8_t rst = 0x40U;
    uint8_t osr = 0x1CU;
    uint8_t hms = 0x03U;
    value = rst | osr | hms;
    status = I2C::writeRegister(FXOS_ADDRESS, FXOS_M_CTRL_REG_1, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Enable hyrid mode auto increment
    value = 0x20U;
    status = I2C::writeRegister(FXOS_ADDRESS, FXOS_M_CTRL_REG_2, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Set range
    value = fxosRange;
    status = I2C::writeRegister(FXOS_ADDRESS, FXOS_XYZ_DATA_CFG, &value);
    if (status != kStatus_Success) {
        return status;
    }

    return status;
}

status_t IMU::startFXOS() {
    // Active, low noise, ODR 400 Hz
    uint8_t active = 0x01U;
    uint8_t dr = 0x00U;
    uint8_t lnoise = 0x04U;

    if (fxosRange == FXOSRange::G_8) {
        lnoise = 0;
    }

    uint8_t value = active | dr | lnoise;
    return I2C::writeRegister(FXOS_ADDRESS, FXOS_CTRL_REG_1, &value);
}

void IMU::callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status,
                   void *userData) {
    IMU *imu = reinterpret_cast<IMU *>(userData);
    if (handle->transfer.slaveAddress == FXOS_ADDRESS) {
        uint8_t *buffer = imu->getFXOSBuffer();

        imu->accel.x =
            static_cast<int16_t>(static_cast<uint16_t>(buffer[0] << 8) |
                                 static_cast<uint16_t>(buffer[1]));
        imu->accel.y =
            static_cast<int16_t>(static_cast<uint16_t>(buffer[2] << 8) |
                                 static_cast<uint16_t>(buffer[3]));
        imu->accel.z =
            static_cast<int16_t>(static_cast<uint16_t>(buffer[4] << 8) |
                                 static_cast<uint16_t>(buffer[5]));

        imu->mag.x =
            static_cast<int16_t>(static_cast<uint16_t>(buffer[6] << 8) |
                                 static_cast<uint16_t>(buffer[7]));
        imu->mag.y =
            static_cast<int16_t>(static_cast<uint16_t>(buffer[8] << 8) |
                                 static_cast<uint16_t>(buffer[9]));
        imu->mag.z =
            static_cast<int16_t>(static_cast<uint16_t>(buffer[10] << 8) |
                                 static_cast<uint16_t>(buffer[11]));
    } else {
        uint8_t *buffer = imu->getFXASBuffer();

        imu->gyro.x =
            static_cast<int16_t>(static_cast<uint16_t>(buffer[0] << 8) |
                                 static_cast<uint16_t>(buffer[1]));
        imu->gyro.y =
            static_cast<int16_t>(static_cast<uint16_t>(buffer[2] << 8) |
                                 static_cast<uint16_t>(buffer[3]));
        imu->gyro.z =
            static_cast<int16_t>(static_cast<uint16_t>(buffer[4] << 8) |
                                 static_cast<uint16_t>(buffer[5]));
    }
}

i2c_master_handle_t &IMU::getHandle() { return this->handle; }

uint8_t *IMU::getFXOSBuffer() { return this->fxosBuffer; }

status_t IMU::initFXAS() {
    status_t status;
    uint8_t value = 0;

    // Set stand by mode
    status = I2C::writeRegister(FXAS_ADDRESS, FXAS_CTRL_REG1, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Disable FIFO
    status = I2C::writeRegister(FXAS_ADDRESS, FXAS_F_SETUP, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Enable data ready interrupt and route to INT1
    uint8_t int_en_drdy = 0x04U;
    uint8_t int_cfg_drdy = 0x08U;
    value = int_en_drdy | int_cfg_drdy;
    status = I2C::writeRegister(FXAS_ADDRESS, FXAS_CTRL_REG2, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Configure auto-increment
    value = 0x8U;
    status = I2C::writeRegister(FXAS_ADDRESS, FXAS_CTRL_REG3, &value);
    if (status != kStatus_Success) {
        return status;
    }

    // Configure range
    value = fxasRange;
    status = I2C::writeRegister(FXAS_ADDRESS, FXAS_CTRL_REG0, &value);
    if (status != kStatus_Success) {
        return status;
    }

    return status;
}

status_t IMU::startFXAS() {
    // Active mode, 400 Hz data rate
    uint8_t active = 0x03U;
    uint8_t dr = 0x04U;
    uint8_t value = dr | active;

    return I2C::writeRegister(FXAS_ADDRESS, FXAS_CTRL_REG1, &value);
}

uint8_t *IMU::getFXASBuffer() { return this->fxasBuffer; }

Shared::Vec3<int16_t> IMU::getAccel() const { return this->accel; }

Shared::Vec3<int16_t> IMU::getMag() const { return this->mag; }

Shared::Vec3<int16_t> IMU::getGyro() const { return this->gyro; }