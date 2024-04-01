/*
 * I2C.h
 *
 *  Created on: Mar 30, 2024
 *      Author: gaito
 */

#ifndef I2C_H_
#define I2C_H_

#include <cstdint>

#include "fsl_common.h"

namespace MCU::I2C {

void BOARD_I2C_ReleaseBus();

status_t readRegister(uint8_t deviceAddress, uint8_t registerAddress,
                      uint8_t *buffer, uint32_t size);

status_t writeRegister(uint8_t deviceAddress, uint8_t registerAddress,
                       uint8_t *buffer, uint32_t size = 1);
} // namespace MCU::I2C

#endif /* I2C_H_ */