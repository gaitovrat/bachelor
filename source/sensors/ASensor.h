/*
 * ASensor.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef SENSORS_ASENSOR_H_
#define SENSORS_ASENSOR_H_

#include <vector>

#include "../Pin.h"

class ASensor {
	static bool sInitialized;

public:
	ASensor(I2C_Type *pBase);

	virtual ~ASensor() = default;

	void Init();

	uint8_t SlaveAddress() const;

	int Data() const;

	virtual Pin& SDAPin() const = 0;

	virtual Pin& SCLPin() const = 0;

	virtual uint8_t WhoAmIRegister() const = 0;

	virtual const std::vector<uint8_t>& Addresses() const = 0;

private:
	void ReadWhoAmI();

	void BOARD_I2C_ReleaseBus();

	I2C_Type *mpBase;
	uint8_t mSlaveAddress;
};

#endif /* SENSORS_ASENSOR_H_ */
