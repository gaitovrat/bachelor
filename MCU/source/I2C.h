#ifndef I2C_H_
#define I2C_H_

#include <cstdint>

#include "fsl_common.h"
#include "fsl_i2c.h"

namespace MCU::I2C {

/*
 * Release the I2C bus
 */
void BOARD_I2C_ReleaseBus();

/*
 * Read device register
 * @param deviceAddress the device address
 * @param registerAddress the register address
 * @param buffer the buffer to read into
 * @param size the size of the buffer
 * @return the status of the operation
 */
status_t readRegister(uint8_t deviceAddress, uint8_t registerAddress,
                      uint8_t *buffer, uint32_t size);

/*
 * Write value to device register
 * @param deviceAddress the device address
 * @param registerAddress the register address
 * @param buffer the buffer contains value to write
 * @param size the size of the buffer
 * @return the status of the operation
 */
status_t writeRegister(uint8_t deviceAddress, uint8_t registerAddress,
                       uint8_t *buffer, uint32_t size = 1);
}  // namespace MCU::I2C

#endif /* I2C_H_ */
