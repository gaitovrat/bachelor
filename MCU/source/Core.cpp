#include "Core.h"

#include <cmath>
#include <cstdint>

#include "fsl_debug_console.h"

#include "Shared/Image.h"
#include "Shared/Mode.h"
#include "Utils.h"
#include "tfc_k6xf.h"

#ifndef PI
#define PI 3.14
#endif

static constexpr uint32_t TRACER_HISTORY_SIZE = 5;

using namespace MCU;

Core::Core()
    : settings(), motorState(Shared::MotorState::Stop),
      tracer(TRACER_HISTORY_SIZE), pidData(settings.pidData),
      pid(pidData, P_ON_E, DIRECT) {
    this->pid.SetMode(AUTOMATIC);
}

void Core::init() {
    tfc.InitAll();

    tfc.setLEDs(0b1111);
#ifdef USE_ENET
    enet.init(sizeof(Shared::Data), 5000);
#endif
#if 0
    fxos.init();
    fxas.init();
#endif
    imu.init();

    tfc.MotorPWMOnOff(true);
    tfc.ServoOnOff(true);
    tfc.setPWMMax(settings.maxSpeed);

    tfc.setLEDs(0);
}

void Core::calibrate() {
    switch (tfc.getDIPSwitch() >> 1) {
    case 0b100: {
        tfc.MotorPWMOnOff(false);
        tfc.ServoOnOff(true);
        tfc.setServo_i(0, 0);

        int pa = tfc.ReadPot_i(0);
        int pb = tfc.ReadPot_i(1);

        tfc.setLEDs(ledCenter(pa) | (ledCenter(pb) << 2));
        PRINTF("DutyCycle: %4d %4d\r\n", TFC_SERVO_DEFAULT_CENTER + pa,
               TFC_SERVO_DEFAULT_CENTER + pb);

        // Workaround for diagnostic purpose only! Do not use it for control!
        // Temporarily enable full servo control
        tfc_setting_s curset = tfc.setting;
        tfc.setting.servo_center[0] = TFC_SERVO_DEFAULT_CENTER + pa;
        tfc.setting.servo_max_lr[0] = TFC_ANDATA_MINMAX;
        tfc.setting.servo_center[1] = TFC_SERVO_DEFAULT_CENTER + pb;
        tfc.setting.servo_max_lr[1] = TFC_ANDATA_MINMAX;

        tfc.setServo_i(0, 0);
        tfc.setServo_i(1, 0);

        // restore safe control
        tfc.setting = curset;
    } break;
    case 0b010: {
        tfc.MotorPWMOnOff(true);
        tfc.ServoOnOff(false);
        tfc.setServo_i(0, 0);

        int pa = tfc.ReadPot_i(0);
        int pb = tfc.ReadPot_i(1);
        tfc.setLEDs(ledCenter(pa) | (ledCenter(pb) << 2));
        PRINTF("Pot1: %5d Pot2: %5d FB-A: %6.2f FB-B: %6.2f\r\n", pa, pb,
               tfc.ReadFB_f(0), tfc.ReadFB_f(1));

        tfc_setting_s curset = tfc.setting;
        tfc.setting.pwm_max = HW_TFC_PWM_MAX;
        tfc.setMotorPWM_i(pa, pb);

        tfc.setting = curset;
    } break;
    case 0b001: {
        tfc.setServo_i(0, 0);
        tfc.MotorPWMOnOff(0);
        tfc.setLEDs(0);

        uint16_t line[TFC_CAMERA_LINE_LENGTH];

        tfc.getImage(0, line, TFC_CAMERA_LINE_LENGTH);

        PRINTF("Line:\r\n");
        for (uint16_t i = 0; i < TFC_CAMERA_LINE_LENGTH; i++) {
            if (i % 16 == 0)
                PRINTF("\r\n");
            PRINTF(" %03X", line[i]);
        }
        PRINTF("\r\n");
        PRINTF("%lX\r\n", tfc.getDIPSwitch());
    } break;
    default: {
        tfc.setServo_i(0, 0);
        tfc.MotorPWMOnOff(0);

        PRINTF("SwA: %ld SwB: %ld Pot1: %5ld Pot2: %5ld BAT: %5.2f\r\n",
               tfc.getPushButton(0), tfc.getPushButton(1), tfc.ReadPot_i(0),
               tfc.ReadPot_i(1), tfc.ReadBatteryVoltage_f());

        uint8_t bat = 0;
        if (tfc.getPushButton(0))
            bat |= 0x3;
        if (tfc.getPushButton(1))
            bat |= 0xC;
        tfc.setLEDs(bat);
    } break;
    }
}

void Core::drive() {
    static uint32_t oldTick = 0;
    int32_t servoPosition = 0, leftSpeed = 0, rightSpeed = 0;
#ifdef USE_ENET
    this->enet.check();
#endif

    if (this->tfc.getPushButton(0) == 1) {
        if (this->motorState == Shared::MotorState::Start)
            this->motorState = Shared::MotorState::Stop;
        else
            this->motorState = Shared::MotorState::Start;
    }

    if (HW_TFC_TimeStamp != oldTick) {
#ifdef USE_TFC
        if (this->tfc.getDIPSwitch() & 0x01)
            this->calibrate();
        else {
#endif
            this->update(servoPosition, leftSpeed, rightSpeed);
            this->send();
#ifdef USE_TFC
        }
#endif

        this->tfc.setServo_i(0, servoPosition);
        this->tfc.setMotorPWM_i(leftSpeed, rightSpeed);
        oldTick = HW_TFC_TimeStamp;
    }
}

void Core::update(int32_t &servoPosition, int32_t &leftSpeed,
                  int32_t &rightSpeed) {
    Shared::Image::ImageLine line;
    float ratio = 0.f, innerSpeed = 0.f, outerSpeed = 0.f, turningAngle = 0.f;

    if (this->motorState == Shared::MotorState::Stop) {
        this->tfc.setLEDs(0);
        this->reset();

        servoPosition = 0;
        leftSpeed = 0;
        rightSpeed = 0;

        return;
    }

    this->tfc.setLEDs(0b1001);
    this->tfc.getImage(0, line, Shared::Image::LINE_LENGTH);
    this->calculateDistanceRatio(line, ratio);

    pidData.input = ratio;
    pid.Compute();
    servoPosition = static_cast<int32_t>(pidData.output);

    if (servoPosition > TFC_SERVO_MINMAX)
        servoPosition = TFC_SERVO_MINMAX;
    if (servoPosition < -TFC_SERVO_MINMAX)
        servoPosition = -TFC_SERVO_MINMAX;

    turningAngle =
        (servoPosition * 5.85f / 200) * PI / 180.f; // Convert servo to angle
    innerSpeed =
        settings.speed *
        (1.f - settings.diffCoef * (1.50f * tanf(turningAngle)) / 2.f * 1.85f);
    outerSpeed =
        settings.speed *
        (1.f + settings.diffCoef * (1.50f * tanf(turningAngle)) / 2.f * 1.85f);

    if (!(tracer.getUnchangedRight() && tracer.getUnchangedLeft())) {
        if (turningAngle > 0.f) {
            rightSpeed = innerSpeed;
            leftSpeed = outerSpeed;
        } else {
            leftSpeed = innerSpeed;
            rightSpeed = outerSpeed;
        }

        leftSpeed *= 0.75f;
        rightSpeed *= 0.75f;
        servoPosition *= 1.5f;
    }
}

void Core::reset() {
    tracer.reset();

    pidData.reset();
    pid = PID(pidData, P_ON_E, DIRECT);
}

void Core::send() {
    Shared::Data data;
    data.timestamp = HW_TFC_TimeStamp;

    // Sensors
    data.sensorData.accel = imu.getAccel();
    data.sensorData.mag = imu.getMag();
    data.sensorData.gyro = imu.getGyro();

#ifdef USE_ENET
    uint8_t *pData = reinterpret_cast<uint8_t *>(&data);
    enet.send(pData, sizeof(data));
#else
    PRINTF("Accel: %d,%d,%d; Mag: %d,%d,%d; Gyro: %d,%d,%d\r\n",
           data.sensorData.accel.x, data.sensorData.accel.y,
           data.sensorData.accel.z, data.sensorData.mag.x,
           data.sensorData.mag.y, data.sensorData.mag.z, data.sensorData.gyro.x,
           data.sensorData.gyro.y, data.sensorData.gyro.z);
#endif
}

void Core::calculateDistanceRatio(Shared::Image::RefImageLine image,
                                  float &ratio) {
    this->tracer.addImage(image);

    std::pair<uint8_t, uint8_t> distances = tracer.distancesPair();

    const uint8_t leftDistance = distances.first;
    const uint8_t rightDistance = Shared::Image::LINE_LENGTH - distances.second;

    const float leftRatio =
        static_cast<float>(leftDistance) / static_cast<float>(rightDistance);
    const float rightRatio =
        static_cast<float>(rightDistance) / static_cast<float>(leftDistance);

    ratio = rightRatio - leftRatio;
}

IMU &Core::getIMU() { return this->imu; }
