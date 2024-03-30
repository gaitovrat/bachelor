/*
 * IMU.h
 *
 *  Created on: Mar 30, 2024
 *      Author: gaito
 */

#ifndef IMU_H_
#define IMU_H_

#include "fsl_common.h"
#include "fsl_i2c.h"

namespace MCU {
class IMU {
  public:
    enum FXOSRange { G_2 = 0, G_4, G_8 };

  private:
    static constexpr uint8_t FXOS_ADDRESS = 0x1DU;

    const FXOSRange range;
    i2c_master_handle_t handle;

  public:
    IMU(FXOSRange range = G_4);

    virtual ~IMU() = default;

    status_t init();

  private:
    status_t initFXOS();

    status_t startFXOS();

    void initMaster();

    static void callback(I2C_Type *base, i2c_master_handle_t *handle,
                         status_t status, void *userData);
};
} // namespace MCU

#endif /* IMU_H_ */
