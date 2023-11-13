/*
 * ASensor.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef SENSORS_ASENSOR_H_
#define SENSORS_ASENSOR_H_

#include <vector>
#include <cstdint>

#include <fsl_i2c.h>

class ASensor {
public:
	ASensor(I2C_Type *pBase);

	virtual ~ASensor() = default;

	virtual void Init();

	void FindSlaveAddress();

	uint8_t SlaveAddress() const;

	virtual uint8_t WhoAmIRegister() const = 0;

	virtual const std::vector<uint8_t>& SlaveAddresses() const = 0;

private:
	I2C_Type *mpBase;
	uint8_t mSlaveAddress;
};

#endif /* SENSORS_ASENSOR_H_ */
