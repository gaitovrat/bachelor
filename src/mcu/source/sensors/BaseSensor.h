/*
 * ASensor.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef SENSORS_BASESENSOR_H_
#define SENSORS_BASESENSOR_H_

#include <cstdint>

#include <fsl_i2c.h>

class BaseSensor {
	static bool initialized;
	I2C_Type *base;

public:
	BaseSensor(I2C_Type *base);

	virtual ~BaseSensor() = default;

	virtual status_t init();

	virtual uint8_t getDeviceAddress() const = 0;

	status_t readRegister(uint8_t registerAddress, uint8_t *buffer, size_t bufferSize) const;

	status_t writeRegister(uint8_t registerAddress, uint8_t *buffer);
};

#endif /* SENSORS_BASESENSOR_H_ */
