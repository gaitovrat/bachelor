#ifndef CORE_H_
#define CORE_H_

#include "Enet.h"
#include "IMU.h"
#include "LineTracer.h"
#include "PID.h"
#include "Shared/Data.h"
#include "Shared/Filter.h"
#include "tfc.h"

namespace MCU {
class Core {
    static constexpr float ERROR = 160.6f;
    static constexpr float DERIVATIVE = 8.3f;
    static constexpr float INTEGRAL = 0.5f;
    static constexpr float DIFF_COEF = 1.28f;
    static constexpr uint16_t MAX_SPEED = 300;
    static constexpr uint16_t MIN_SPEED = 230;
    static constexpr uint32_t TRACER_HISTORY_SIZE = 5;
    static constexpr uint32_t SERVO_CENTER = 1500;
    static constexpr uint32_t SERVO_LR_NS = 750;
    static constexpr uint32_t SERVO_LR = 1000;
    static constexpr uint32_t PWM_STEP = 10;

    TFC tfc;
    Enet enet;
    LineTracer tracer;

    Shared::PIDData pidData;
    PID pid;

    Shared::Data data;

    bool previousButtonState;

    IMU imu;
    Shared::Filter gyroFilter;
    Shared::Filter accelFilter;
    float prevGyro;

    int32_t prevServoPosition;
    float speed;

public:
    /*
     * Constructor
     */
    Core();

    /*
     * Destructor
     */
    virtual ~Core() = default;

    /*
     * Initialize the Core
     */
    void init();

    /*
     * Calibrate MCU
     */
    void calibrate();

    /*
     * Run MCU
     */
    void drive();

    /*
     * Get the IMU
     * @return the IMU
     */
    IMU &getIMU();

    /*
     * Get the gyro filter
     */
    Shared::Filter &getGyroFilter();

    /*
     * Get the accel filter
     */
    Shared::Filter &getAccelFilter();

private:
    /*
     * Update drive in automatic mode
     */
    void update();

    /*
     * Update drive in manual mode
     */
    void manual();

    /*
     * Reset the Core
     */
    void reset();

    /*
     * Send data to PC
     */
    void send();

    /*
     * Calculate the distance ratio
     * @return the distance ratio
     */
    float calculateDistanceRatio();
};
}  // namespace MCU

#endif /* CORE_H_ */
