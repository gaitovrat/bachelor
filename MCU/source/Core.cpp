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

using namespace MCU;

Core::Core()
    : tracer(TRACER_HISTORY_SIZE), pidData(ERROR, INTEGRAL, DERIVATIVE),
      pid(pidData, P_ON_E, DIRECT) {
    this->pid.SetMode(AUTOMATIC);
}

void Core::init() {
    tfc.InitAll();
    tfc.InitRC();

    tfc.setLEDs(0b1111);
    enet.init(sizeof(Shared::Data), 5000);
    imu.init();

    tfc.MotorPWMOnOff(true);
    tfc.ServoOnOff(true);
    tfc.RCOnOff(true);
    tfc.setPWMMax(MAX_SPEED);

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
    this->enet.check();
    this->tfc.setLEDs(this->data.mode);

    if (HW_TFC_TimeStamp != this->data.timestamp) {
        if (this->tfc.getDIPSwitch() & 0x01)
            this->calibrate();
        else if (this->data.mode == Shared::Mode::Manual) {
            this->manual();
        } else if (this->data.mode == Shared::Mode::Auto) {
            this->update();
        } else {
            this->reset();
        }

        this->send();
        this->tfc.setServo_i(0, this->data.servoPosition);
        this->tfc.setMotorPWM_i(this->data.leftSpeed, this->data.rightSpeed);

        this->data.timestamp = HW_TFC_TimeStamp;
    }
}

void Core::update() {
    float ratio, innerSpeed, outerSpeed;

    ratio = this->calculateDistanceRatio();

    pidData.input = ratio;
    pid.Compute();
    data.servoPosition = static_cast<int32_t>(pidData.output);

    if (data.servoPosition > TFC_SERVO_MINMAX)
        data.servoPosition = TFC_SERVO_MINMAX;
    if (data.servoPosition < -TFC_SERVO_MINMAX)
        data.servoPosition = -TFC_SERVO_MINMAX;

    data.angle = (data.servoPosition * 5.85f / 200) * PI /
                 180.f; // Convert servo to angle
    innerSpeed =
        SPEED * (1.f - DIFF_COEF * (1.50f * tanf(data.angle)) / 2.f * 1.85f);
    outerSpeed =
        SPEED * (1.f + DIFF_COEF * (1.50f * tanf(data.angle)) / 2.f * 1.85f);

    if (!(tracer.getUnchangedRight() && tracer.getUnchangedLeft())) {
        if (data.angle > 0.f) {
            data.rightSpeed = innerSpeed;
            data.leftSpeed = outerSpeed;
        } else {
            data.leftSpeed = innerSpeed;
            data.rightSpeed = outerSpeed;
        }

        data.leftSpeed *= 0.75f;
        data.rightSpeed *= 0.75f;
        data.servoPosition *= 1.5f;
    }
}

void Core::reset() {
    tracer.reset();

    pidData.reset();
    pid = PID(pidData, P_ON_E, DIRECT);

    uint32_t timestamp = data.timestamp;
    data = Shared::Data();
    data.timestamp = timestamp;
}

void Core::send() {
    data.accel = imu.getAccel();
    data.mag = imu.getMag();
    data.gyro = imu.getGyro();

    uint8_t *pData = reinterpret_cast<uint8_t *>(&data);
    enet.send(pData, sizeof(data));
}

float Core::calculateDistanceRatio() {
    this->tfc.getImage(0, data.line, Shared::Image::LINE_LENGTH);
    this->tracer.addImage(data.line);

    std::pair<uint8_t, uint8_t> distances = tracer.distancesPair();

    data.leftDistance = distances.first;
    data.rightDistance = Shared::Image::LINE_LENGTH - distances.second;

    const float leftRatio = static_cast<float>(data.leftDistance) /
                            static_cast<float>(data.rightDistance);
    const float rightRatio = static_cast<float>(data.rightDistance) /
                             static_cast<float>(data.leftDistance);

    data.regionsCount =
        tracer.regions(data.line, 0, TFC_CAMERA_LINE_LENGTH - 1, false).size();
    data.regionsListSize = tracer.size();
    data.unchangedLeft = tracer.getUnchangedLeft();
    data.unchangedRight = tracer.getUnchangedRight();
    data.hasLeft = tracer.getHasLeft();
    data.hasRight = tracer.getHasRight();

    return rightRatio - leftRatio;
}

IMU &Core::getIMU() { return this->imu; }

void Core::manual() {
    int32_t servo = this->tfc.getRCPulse(0);
    int32_t speed = this->tfc.getRCPulse(1);

    if (!servo || !speed)
        return;

    float servof = servo / 100.f - 15;
    float speedf = speed / 100.f - 15;
    servof *= 2;
    speedf *= 2;

    int32_t newSpeed = this->tfc.setting.pwm_max * (speedf / 10.f) * -1;

    this->data.servoPosition = TFC_SERVO_MINMAX * (servof / 10.f);
    this->data.leftSpeed = newSpeed;
    this->data.rightSpeed = newSpeed;
}

void Core::setMode(Shared::Mode mode) { this->data.mode = mode; }
