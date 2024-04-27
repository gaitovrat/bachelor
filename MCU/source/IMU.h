#ifndef IMU_H_
#define IMU_H_

#include "Shared/Vec.h"
#include "fsl_common.h"
#include "fsl_i2c.h"

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
    Shared::Vec3<int16_t> accel;
    Shared::Vec3<int16_t> mag;
    Shared::Vec3<int16_t> gyro;
    bool started;

public:
    IMU(FXOSRange fxosRange = G_4, FXASRange fxasRange = DPS_1000);

    virtual ~IMU() = default;

    void init();

    Shared::Vec3<int16_t> getAccel() const;

    Shared::Vec3<int16_t> getMag() const;

    Shared::Vec3<int16_t> getGyro() const;

    void setAccel(const Shared::Vec3<int16_t>& vec);

    void setMag(const Shared::Vec3<int16_t>& vec);

    void setGyro(const Shared::Vec3<int16_t>& vec);

    void start();

private:
    status_t initFXOS();

    status_t startFXOS();

    status_t initFXAS();

    status_t startFXAS();

    void initMaster();
};
}  // namespace MCU

#endif /* IMU_H_ */
