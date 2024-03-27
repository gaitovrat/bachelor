/*
 * ASensor.cpp
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#include "BaseSensor.h"

#include <cstring>

#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

#define I2C_RELEASE_SDA_PORT PORTE
#define I2C_RELEASE_SCL_PORT PORTE
#define I2C_RELEASE_SDA_GPIO GPIOE
#define I2C_RELEASE_SDA_PIN 25U
#define I2C_RELEASE_SCL_GPIO GPIOE
#define I2C_RELEASE_SCL_PIN 24U
#define I2C_RELEASE_BUS_COUNT 100U
#define BAUDRATE 100000;

using namespace MCU;

bool BaseSensor::initialized = false;

static void i2c_release_bus_delay(void) {
    uint32_t i = 0;
    for (i = 0; i < I2C_RELEASE_BUS_COUNT; i++) {
        __NOP();
    }
}

static void BOARD_I2C_ReleaseBus() {
    uint8_t i = 0;
    gpio_pin_config_t pin_config;
    port_pin_config_t i2c_pin_config = {0};

    /* Config pin mux as gpio */
    i2c_pin_config.pullSelect = kPORT_PullUp;
    i2c_pin_config.mux = kPORT_MuxAsGpio;

    pin_config.pinDirection = kGPIO_DigitalOutput;
    pin_config.outputLogic = 1U;
    CLOCK_EnableClock(kCLOCK_PortE);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SCL_PIN,
                      &i2c_pin_config);
    PORT_SetPinConfig(I2C_RELEASE_SDA_PORT, I2C_RELEASE_SDA_PIN,
                      &i2c_pin_config);

    GPIO_PinInit(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, &pin_config);
    GPIO_PinInit(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, &pin_config);

    /* Drive SDA low first to simulate a start */
    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    /* Send 9 pulses on SCL and keep SDA high */
    for (i = 0; i < 9; i++) {
        GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
        i2c_release_bus_delay();

        GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
        i2c_release_bus_delay();

        GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    /* Send stop */
    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
    i2c_release_bus_delay();
}

BaseSensor::BaseSensor(I2C_Type *base) : base(base), handle{} {}

status_t BaseSensor::init() {
    status_t status = 0;

    if (BaseSensor::initialized) {
        return kStatus_Success;
    }

    BOARD_I2C_ReleaseBus();

    i2c_master_config_t masterConfig;
    uint32_t sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);

    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = BAUDRATE;
    I2C_MasterInit(this->base, &masterConfig, sourceClock);

    status = this->setupRegisters();

    I2C_MasterTransferCreateHandle(this->base, &this->handle,
                                   BaseSensor::transferCallback, (void *)this);

    BaseSensor::initialized = true;

    return status;
}

status_t BaseSensor::readRegister(uint8_t registerAddress, uint8_t *buffer,
                                  size_t bufferSize) const {
    const uint8_t deviceAddress = this->deviceAddress();

    i2c_master_transfer_t masterXfer;
    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = deviceAddress;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress = registerAddress;
    masterXfer.subaddressSize = 1;
    masterXfer.data = buffer;
    masterXfer.dataSize = bufferSize;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(this->base, &masterXfer);
}

status_t BaseSensor::writeRegister(uint8_t registerAddress, uint8_t *buffer) {
    const uint8_t deviceAddress = this->deviceAddress();

    i2c_master_transfer_t masterXfer;
    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = deviceAddress;
    masterXfer.direction = kI2C_Write;
    masterXfer.subaddress = registerAddress;
    masterXfer.subaddressSize = 1;
    masterXfer.data = buffer;
    masterXfer.dataSize = 1;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(this->base, &masterXfer);
}

void BaseSensor::transferCallback(I2C_Type *base, i2c_master_handle_t *handle,
                                  status_t status, void *userData) {
    PRINTF("callback\r\n");
}

void BaseSensor::handler() {
    i2c_master_transfer_t masterXfer;
    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.flags = kI2C_TransferDefaultFlag;
    masterXfer.slaveAddress = deviceAddress();
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddressSize = 1;
    masterXfer.data = getData();
    masterXfer.dataSize = getDataSize();
    masterXfer.subaddress = getDataRegister();

    I2C_MasterTransferNonBlocking(base, &handle, &masterXfer);
}
