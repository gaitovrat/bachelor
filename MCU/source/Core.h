#ifndef CORE_H_
#define CORE_H_

#include "Enet.h"
#include "LineTracer.h"
#include "PID.h"
#include "Sensor/FXAS21002.h"
#include "Sensor/FXOS8700CQ.h"
#include "Sensor/IRSensor.h"
#include "Shared/Data.h"
#include "Shared/MotorState.h"
#include "Shared/Settings.h"
#include "tfc.h"

namespace MCU {
class Core {
    TFC tfc;
    Enet enet;
    Shared::Settings settings;
    Shared::MotorState motorState;
    LineTracer tracer;
    Shared::PIDData pidData;
    PID pid;
    FXOS8700CQ fxos;
    FXAS21002 fxas;
    IRSensor ir;

  public:
    Core();

    virtual ~Core() = default;

    void init();

    void calibrate();

    void drive();

  private:
    void update(int32_t &servoPosition, int32_t &leftSpeed,
                int32_t &rightSpeed);

    void reset();

    void setRide();

    void send();

    void calculateDistanceRatio(Shared::Image::RefImageLine image,
                                float &ratio);

    uint32_t calculateServoPosition(float ratio);
};
} // namespace MCU

#endif /* CORE_H_ */
