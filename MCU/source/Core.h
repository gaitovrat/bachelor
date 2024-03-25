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

    // Motor
    Shared::PIDData pidData;
    PID pid;
    int speed;

    // Servo
    float steerSetting;
    int16_t prevServoPosition;
    int16_t servoPosition;
    uint8_t left;
    uint8_t right;

    Shared::Data data;

    // Sensors
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
    void update();

    void reset();

    void setRide();

    void sendData();
};
} // namespace MCU

#endif /* CORE_H_ */
