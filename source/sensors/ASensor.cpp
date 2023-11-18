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

static constexpr uint32_t sBaudrate = 100000;
static const uint32_t sClockFreq = CLOCK_GetFreq(I2C0_CLK_SRC);

ASensor::ASensor(I2C_Type *pBase) : mp_Base(pBase) {
}

void ASensor::Init() {
	i2c_master_config_t masterConfig;
	uint32_t sourceClock = sClockFreq;

	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = sBaudrate;
	I2C_MasterInit(mp_Base, &masterConfig, sourceClock);
}

status_t ASensor::ReadRegister(uint8_t registerAddress, uint8_t *pBuffer, size_t bufferSize) const {
	const uint8_t deviceAddress = DeviceAddress();

	i2c_master_transfer_t masterXfer;
	memset(&masterXfer, 0, sizeof(masterXfer));

	masterXfer.slaveAddress = deviceAddress;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = registerAddress;
	masterXfer.subaddressSize = sizeof(registerAddress);
	masterXfer.data = pBuffer;
	masterXfer.dataSize = bufferSize;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	return I2C_MasterTransferBlocking(mp_Base, &masterXfer);
}

status_t ASensor::WriteRegister(uint8_t registerAddress, uint8_t *pBuffer, size_t bufferSize) {
	const uint8_t deviceAddress = DeviceAddress();

	i2c_master_transfer_t masterXfer;
	memset(&masterXfer, 0, sizeof(masterXfer));

	masterXfer.slaveAddress = deviceAddress;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = registerAddress;
	masterXfer.subaddressSize = sizeof(registerAddress);
	masterXfer.data = pBuffer;
	masterXfer.dataSize = bufferSize;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	return I2C_MasterTransferBlocking(mp_Base, &masterXfer);
}
