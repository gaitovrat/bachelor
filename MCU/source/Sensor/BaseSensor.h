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
    static bool initialized;
    I2C_Type *base;
    i2c_master_handle_t handle;

  public:
    BaseSensor(I2C_Type *base);

    virtual ~BaseSensor() = default;

    virtual status_t init();

    virtual status_t setupRegisters() = 0;

    virtual uint8_t deviceAddress() const = 0;

    virtual uint8_t getDataRegister() const = 0;

    virtual uint8_t *getData() = 0;

    virtual uint32_t getDataSize() const = 0;

    status_t readRegister(uint8_t registerAddress, uint8_t *buffer,
                          size_t bufferSize) const;

    status_t writeRegister(uint8_t registerAddress, uint8_t *buffer);

    void handler();

    static void transferCallback(I2C_Type *base, i2c_master_handle_t *handle,
                                 status_t status, void *userData);
};
} // namespace MCU

#endif /* SENSORS_BASESENSOR_H_ */
