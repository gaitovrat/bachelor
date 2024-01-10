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
	static bool initialized;

	I2C_Type *base;

public:
	ASensor(I2C_Type *base);

	virtual ~ASensor() = default;

	virtual status_t init();

	virtual uint8_t getDeviceAddress() const = 0;

	status_t readRegister(uint8_t registerAddress, uint8_t *buffer, size_t bufferSize) const;

	status_t writeRegister(uint8_t registerAddress, uint8_t *buffer, size_t bufferSize);
};

#endif /* SENSORS_ASENSOR_H_ */
