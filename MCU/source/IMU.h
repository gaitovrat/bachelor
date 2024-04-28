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
    /*
     * Constructor
     * @param fxosRange the FXOS range
     * @param fxasRange the FXAS range
     */
    IMU(FXOSRange fxosRange = G_4, FXASRange fxasRange = DPS_1000);

    /*
     * Destructor
     */
    virtual ~IMU() = default;

    /*
     * Initialize the IMU
     */
    void init();

    /*
     * Get the accelerometer data
     * @return the accelerometer data
     */
    Shared::Vec3<int16_t> getAccel() const;

    /*
     * Get the magnetometer data
     * @return the magnetometer data
     */
    Shared::Vec3<int16_t> getMag() const;

    /*
     * Get the gyroscope data
     * @return the gyroscope data
     */
    Shared::Vec3<int16_t> getGyro() const;

    /*
     * Set the accelerometer data
     * @param vec the accelerometer data
     */
    void setAccel(const Shared::Vec3<int16_t>& vec);

    /*
     * Set the magnetometer data
     * @param vec the magnetometer data
     */
    void setMag(const Shared::Vec3<int16_t>& vec);

    /*
     * Set the gyroscope data
     * @param vec the gyroscope data
     */
    void setGyro(const Shared::Vec3<int16_t>& vec);

    /*
     * Start the IMU
     */
    void start();

private:
    /*
     * Initialize the FXOS
     * @return the status
     */
    status_t initFXOS();

    /*
     * Start the FXOS
     * @return the status
     */
    status_t startFXOS();

    /*
     * Initialize the FXAS
     * @return the status
     */
    status_t initFXAS();

    /*
     * Initialize the FXOS
     * @return the status
     */
    status_t startFXAS();

    /*
     * Initialize master device
     */
    void initMaster();
};
}  // namespace MCU

#endif /* IMU_H_ */
