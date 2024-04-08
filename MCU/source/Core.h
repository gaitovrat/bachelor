#ifndef CORE_H_
#define CORE_H_

#include "Enet.h"
#include "IMU.h"
#include "LineTracer.h"
#include "PID.h"
#include "Shared/Data.h"
#include "tfc.h"

namespace MCU {
class Core {
    static constexpr float ERROR = 160.6f;
    static constexpr float DERIVATIVE = 8.3f;
    static constexpr float INTEGRAL = 0.5f;
    static constexpr float DIFF_COEF = 1.28f;
    static constexpr uint16_t MAX_SPEED = 1000;
    static constexpr uint16_t SPEED = 300;
    static constexpr uint32_t TRACER_HISTORY_SIZE = 5;

    TFC tfc;
    Enet enet;
    IMU imu;
    LineTracer tracer;

    Shared::PIDData pidData;
    PID pid;

    Shared::Data data;

    bool previousButtonState;

  public:
    Core();

    virtual ~Core() = default;

    void init();

    void calibrate();

    void drive();

    IMU &getIMU();

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
