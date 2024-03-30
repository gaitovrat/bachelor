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

#include "Shared/Vec.h"

namespace MCU {
class IMU {
  public:
    enum FXOSRange { G_2 = 0, G_4, G_8 };
    enum FXASRange { DPS_2000 = 0, DPS_1000, DPS_500, DPS_250 };

    static constexpr uint8_t FXOS_ADDRESS = 0x1DU;
    static constexpr uint8_t FXAS_ADDRESS = 0x21U;

  private:
    const FXOSRange fxosRange;
    const FXASRange fxasRange;
    i2c_master_handle_t handle;
    uint8_t fxosBuffer[12];
    uint8_t fxasBuffer[6];
    Shared::Vec3<int16_t> accel;
    Shared::Vec3<int16_t> mag;
    Shared::Vec3<int16_t> gyro;

  public:
    IMU(FXOSRange fxosRange = G_4, FXASRange fxasRange = DPS_1000);

    virtual ~IMU() = default;

    status_t init();

    i2c_master_handle_t &getHandle();

    uint8_t *getFXOSBuffer();

    uint8_t *getFXASBuffer();

    inline constexpr uint32_t getFXOSBufferSize() const {
        return sizeof(fxosBuffer);
    }

    inline constexpr uint32_t getFXASBufferSize() const {
        return sizeof(fxasBuffer);
    }

    Shared::Vec3<int16_t> getAccel() const;

    Shared::Vec3<int16_t> getMag() const;

    Shared::Vec3<int16_t> getGyro() const;

  private:
    status_t initFXOS();

    status_t startFXOS();

    status_t initFXAS();

    status_t startFXAS();

    void initMaster();

    static void callback(I2C_Type *base, i2c_master_handle_t *handle,
                         status_t status, void *userData);
};
} // namespace MCU

#endif /* IMU_H_ */
