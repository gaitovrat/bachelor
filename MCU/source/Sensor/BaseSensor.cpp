/*
 * ASensor.cpp
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#include "BaseSensor.h"

#include <cstring>

static constexpr uint32_t BAUDRATE = 100000;

bool BaseSensor::s_initialized = false;

BaseSensor::BaseSensor(I2C_Type *base) : m_base(base) {}

status_t BaseSensor::Init() {
	if (BaseSensor::s_initialized) {
		return kStatus_Success;
	}

	i2c_master_config_t masterConfig;
	uint32_t sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);

	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = BAUDRATE;
	I2C_MasterInit(this->m_base, &masterConfig, sourceClock);

	BaseSensor::s_initialized = true;

	return kStatus_Success;
}

status_t BaseSensor::ReadRegister(uint8_t registerAddress, uint8_t *buffer, size_t bufferSize) const {
	const uint8_t deviceAddress = this->DeviceAddress();

	i2c_master_transfer_t masterXfer;
	memset(&masterXfer, 0, sizeof(masterXfer));

	masterXfer.slaveAddress = deviceAddress;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = registerAddress;
	masterXfer.subaddressSize = 1;
	masterXfer.data = buffer;
	masterXfer.dataSize = bufferSize;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	return I2C_MasterTransferBlocking(this->m_base, &masterXfer);
}

status_t BaseSensor::WriteRegister(uint8_t registerAddress, uint8_t *buffer) {
	const uint8_t deviceAddress = this->DeviceAddress();

	i2c_master_transfer_t masterXfer;
	memset(&masterXfer, 0, sizeof(masterXfer));

	masterXfer.slaveAddress = deviceAddress;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = registerAddress;
	masterXfer.subaddressSize = 1;
	masterXfer.data = buffer;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	return I2C_MasterTransferBlocking(this->m_base, &masterXfer);
}
