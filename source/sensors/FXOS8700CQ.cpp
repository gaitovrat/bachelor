/*
 * FXOS8700CQ.cpp
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#include <sensors/FXOS8700CQ.h>

#include <pin_mux.h>

static constexpr uint8_t sWhoAmI = 0x0DU;

FXOS8700CQ::FXOS8700CQ() : ASensor(I2C0),
		mAddresses({0x1CU, 0x1DU, 0x1EU, 0x1FU})
{}

void FXOS8700CQ::Init()
{
	BOARD_InitACCEL_I2CPins();

	ASensor::Init();

	FindSlaveAddress();
}

uint8_t FXOS8700CQ::WhoAmIRegister() const
{
	return sWhoAmI;
}

const std::vector<uint8_t>& FXOS8700CQ::SlaveAddresses() const
{
	return mAddresses;
}
