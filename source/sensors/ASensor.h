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

	virtual uint8_t DeviceAddress() const = 0;

	status_t ReadRegister(uint8_t registerAddress, uint8_t *pBuffer, size_t bufferSize) const;

	status_t WriteRegister(uint8_t registerAddress, uint8_t *pBuffer, size_t bufferSize);

private:
	I2C_Type *mp_Base;
};

#endif /* SENSORS_ASENSOR_H_ */
