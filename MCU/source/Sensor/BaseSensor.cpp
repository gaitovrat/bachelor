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

#if 0
#define BAUDRATE 100000;

using namespace MCU;

bool BaseSensor::initialized = false;

BaseSensor::BaseSensor(I2C_Type *base) : base(base), handle{} {}

status_t BaseSensor::init() {
    status_t status = 0;

    if (BaseSensor::initialized) {
        return kStatus_Success;
    }

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
#endif
