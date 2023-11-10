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

static constexpr uint32_t BAUDRATE = 100000U;
static constexpr uint32_t RELEASE_BUS_COUNT = 100U;

bool ASensor::sInitialized = false;

static void i2c_release_bus_delay()
{
    uint32_t i = 0;
    for (i = 0; i < RELEASE_BUS_COUNT; i++)
    {
        __NOP();
    }
}

void ASensor::BOARD_I2C_ReleaseBus()
{
	// TODO: Implement this for each pin
	// Config pin mux as gpio
	// i2c_pin_config.pullSelect = kPORT_PullUp;
	// i2c_pin_config.mux        = kPORT_MuxAsGpio;
	// pin_config.pinDirection = kGPIO_DigitalOutput;
	// pin_config.outputLogic  = 1U;
    Pin &sclPin = SCLPin();
	Pin &sdaPin = SDAPin();

    CLOCK_EnableClock(kCLOCK_PortD);

    sclPin.Init();
    sdaPin.Init();

    // Drive SDA low first to simulate a start
    sdaPin.Write(0U);
    i2c_release_bus_delay();

    // Send 9 pulses on SCL and keep SDA high
    for (int i = 0; i < 9; i++)
    {
        sclPin.Write(0U);
        i2c_release_bus_delay();

        sdaPin.Write(1U);
        i2c_release_bus_delay();

        sclPin.Write(1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    // Send stop
    sclPin.Write(0U);
    i2c_release_bus_delay();

    sdaPin.Write(0U);
    i2c_release_bus_delay();

    sclPin.Write(1U);
    i2c_release_bus_delay();

    sdaPin.Write(1U);
    i2c_release_bus_delay();
}

ASensor::ASensor(I2C_Type *pBase) : mpBase(pBase), mSlaveAddress(0x00)
{}

void ASensor::Init()
{
	if (!ASensor::sInitialized) {
		BOARD_I2C_ReleaseBus();
	}

	ReadWhoAmI();
}

void ASensor::ReadWhoAmI()
{
	const std::vector<uint8_t> &addresses = Addresses();
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

uint8_t ASensor::SlaveAddress() const {
	return mSlaveAddress;
}

int ASensor::Data() const {
	return 0;
}
