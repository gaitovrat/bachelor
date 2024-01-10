/*
 * ASensor.cpp
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#include <sensors/ASensor.h>

#include <string.h>
#include <assert.h>

#include <fsl_i2c.h>
#include <fsl_debug_console.h>

static constexpr uint32_t BAUDRATE = 100000;

bool ASensor::initialized = false;

ASensor::ASensor(I2C_Type *base) : base(base) {
}

status_t ASensor::init() {
	if (ASensor::initialized) {
		return kStatus_Success;
	}

	i2c_master_config_t masterConfig;
	uint32_t sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);

	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = BAUDRATE;
	I2C_MasterInit(this->base, &masterConfig, sourceClock);

	ASensor::initialized = true;

	return kStatus_Success;
}

status_t ASensor::readRegister(uint8_t registerAddress, uint8_t *buffer, size_t bufferSize) const {
	const uint8_t deviceAddress = this->getDeviceAddress();

	i2c_master_transfer_t masterXfer;
	memset(&masterXfer, 0, sizeof(masterXfer));

	masterXfer.slaveAddress = deviceAddress;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = registerAddress;
	masterXfer.subaddressSize = sizeof(registerAddress);
	masterXfer.data = buffer;
	masterXfer.dataSize = bufferSize;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	return I2C_MasterTransferBlocking(this->base, &masterXfer);
}

status_t ASensor::writeRegister(uint8_t registerAddress, uint8_t *buffer, size_t bufferSize) {
	const uint8_t deviceAddress = this->getDeviceAddress();

	i2c_master_transfer_t masterXfer;
	memset(&masterXfer, 0, sizeof(masterXfer));

	masterXfer.slaveAddress = deviceAddress;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = registerAddress;
	masterXfer.subaddressSize = sizeof(registerAddress);
	masterXfer.data = buffer;
	masterXfer.dataSize = bufferSize;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	return I2C_MasterTransferBlocking(this->base, &masterXfer);
}
