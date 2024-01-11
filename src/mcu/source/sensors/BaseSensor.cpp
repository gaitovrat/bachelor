/*
 * ASensor.cpp
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#include "sensors/BaseSensor.h"

#include <cstring>

static constexpr uint32_t BAUDRATE = 100000;

bool BaseSensor::initialized = false;

BaseSensor::BaseSensor(I2C_Type *base) : base(base) {}

status_t BaseSensor::init() {
	if (BaseSensor::initialized) {
		return kStatus_Success;
	}

	i2c_master_config_t masterConfig;
	uint32_t sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);

	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = BAUDRATE;
	I2C_MasterInit(this->base, &masterConfig, sourceClock);

	BaseSensor::initialized = true;

	return kStatus_Success;
}

status_t BaseSensor::readRegister(uint8_t registerAddress, uint8_t *buffer, size_t bufferSize) const {
	const uint8_t deviceAddress = this->getDeviceAddress();

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
	const uint8_t deviceAddress = this->getDeviceAddress();

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
