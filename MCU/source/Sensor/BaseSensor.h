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

namespace MCU {
class BaseSensor {
  public:
    BaseSensor(I2C_Type *base);

    virtual ~BaseSensor() = default;

    virtual status_t Init();

    virtual uint8_t DeviceAddress() const = 0;

    status_t ReadRegister(uint8_t registerAddress, uint8_t *buffer,
                          size_t bufferSize) const;

    status_t WriteRegister(uint8_t registerAddress, uint8_t *buffer);

  private:
    static bool s_initialized;
    I2C_Type *m_base;
};
} // namespace MCU

#endif /* SENSORS_BASESENSOR_H_ */
