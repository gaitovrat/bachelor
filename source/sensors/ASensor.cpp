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

ASensor::ASensor(I2C_Type *pBase) : mpBase(pBase), mSlaveAddress(0x00)
{}

void ASensor::FindSlaveAddress()
{
	const std::vector<uint8_t> &addresses = SlaveAddresses();
	uint8_t whoAmIRegister = WhoAmIRegister();
	uint8_t whoAmIValue = 0x00;
	status_t status = kStatus_Fail;

	i2c_master_transfer_t masterXfer;
	memset(&masterXfer, 0, sizeof(masterXfer));

	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = &whoAmIRegister;
	masterXfer.dataSize = sizeof(whoAmIRegister);
	masterXfer.flags = kI2C_TransferNoStopFlag;

	for (const uint8_t &address : addresses)
	{
		masterXfer.slaveAddress = address;

		status_t status = I2C_MasterTransferBlocking(mpBase, &masterXfer);

		if (status == kStatus_Success)
		{
			mSlaveAddress = masterXfer.slaveAddress;
			break;
		}
	}

	assert(status == kStatus_Success);

	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = &whoAmIValue;
	masterXfer.dataSize = sizeof(whoAmIValue);
	masterXfer.flags = kI2C_TransferNoStopFlag;

	PRINTF("Found sensor value: 0x%X\r\n", whoAmIValue);
}

uint8_t ASensor::SlaveAddress() const
{
	return mSlaveAddress;
}
