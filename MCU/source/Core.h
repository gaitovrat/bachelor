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
#ifdef NOSENSOR
    static constexpr uint16_t MAX_SPEED = 220;
#else
    static constexpr uint16_t MAX_SPEED = 280;
#endif
    static constexpr uint16_t MIN_SPEED = 200;
    static constexpr uint32_t TRACER_HISTORY_SIZE = 5;
    static constexpr uint32_t SERVO_CENTER = 1500;
    static constexpr uint32_t SERVO_LR = 500;
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
    Core();

    virtual ~Core() = default;

    void init();

    void calibrate();

    void drive();

    IMU &getIMU();

    Shared::Filter &getGyroFilter();

    Shared::Filter &getAccelFilter();

  private:
    void update();

    void manual();

    void reset();

    void send();

    float calculateDistanceRatio();

    uint32_t calculateServoPosition(float ratio);
};
} // namespace MCU

#endif /* CORE_H_ */
