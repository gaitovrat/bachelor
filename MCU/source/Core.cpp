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
      pid(pidData, P_ON_E, DIRECT), ir(tfc) {}

void Core::init() {
    tfc.InitAll();

    tfc.setLEDs(0b1111);
    enet.init(sizeof(Shared::Data), 5000);
    fxos.init();
    fxas.init();

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
        tfc_setting_s curset = tfc.m_setting;
        tfc.m_setting.servo_center[0] = TFC_SERVO_DEFAULT_CENTER + pa;
        tfc.m_setting.servo_max_lr[0] = TFC_ANDATA_MINMAX;
        tfc.m_setting.servo_center[1] = TFC_SERVO_DEFAULT_CENTER + pb;
        tfc.m_setting.servo_max_lr[1] = TFC_ANDATA_MINMAX;

        tfc.setServo_i(0, 0);
        tfc.setServo_i(1, 0);

        // restore safe control
        tfc.m_setting = curset;
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

        tfc_setting_s curset = tfc.m_setting;
        tfc.m_setting.pwm_max = HW_TFC_PWM_MAX;
        tfc.setMotorPWM_i(pa, pb);

        tfc.m_setting = curset;
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

    enet.check();

    if (tfc.getPushButton(0) == 1) {
        if (motorState == Shared::MotorState::Start)
            motorState = Shared::MotorState::Stop;
        else
            motorState = Shared::MotorState::Start;
    }

    if (HW_TFC_TimeStamp != oldTick) {
        if (tfc.getDIPSwitch() & 0x01)
            calibrate();
        else
            update();
        oldTick = HW_TFC_TimeStamp;
    }
}

void Core::update() {
    if (motorState == Shared::MotorState::Start) {
        tfc.setLEDs(0b1001);
    } else {
        tfc.setLEDs(0b0000);
    }

    uint16_t line[Shared::Image::LINE_LENGTH];
    tfc.getImage(0, line, Shared::Image::LINE_LENGTH);
    memcpy(data.cameraData.line, line, Shared::Image::LINE_LENGTH);
    Shared::Image image(line);
    tracer.addImage(image);
    data.cameraData.regionsCount =
        tracer.regions(image, 0, Shared::Image::LINE_LENGTH - 1, false).size();

    auto &distances = tracer.distancesPair();
    data.cameraData.leftDistance = distances.first;
    data.cameraData.rightDistance = distances.second;

    const int leftDistance = data.cameraData.leftDistance;
    const int rightDistance =
        Shared::Image::LINE_LENGTH - data.cameraData.rightDistance;

    const float leftRatio =
        static_cast<float>(leftDistance) / static_cast<float>(rightDistance);
    const float rightRatio =
        static_cast<float>(rightDistance) / static_cast<float>(leftDistance);

    float ratioDiff = rightRatio - leftRatio;

    if (motorState != Shared::MotorState::Stop) {
        pidData.input = ratioDiff;
        pid.Compute();
        steerSetting = static_cast<float>(pidData.output);
    } else {
        steerSetting = 0.f;
    }

    switch (motorState) {
    case Shared::MotorState::Start: {
        speed = 100;
        tfc.setPWMMax(settings.maxSpeed);

        pid.SetMode(AUTOMATIC);
        servoPosition = static_cast<int16_t>(steerSetting);

        break;
    }
    case Shared::MotorState::Stop:
    default: {
        speed = 0;
        pid.SetMode(MANUAL);
        servoPosition = 0;
        tfc.setMotorPWM_i(0, 0);
        reset();
        return;
    }
    }

    motorState = ir.CheckState(motorState);
    setRide();
}

void Core::reset() {
    tracer.reset();

    pidData.reset();
    pid = PID(pidData, P_ON_E, DIRECT);
}

void Core::setRide() {
    if (servoPosition > 1000)
        servoPosition = 1000;
    if (servoPosition < -1000)
        servoPosition = -1000;

    float innerSpeed = 0.f, outerSpeed = 0.f;
    float baseSpeed = settings.maxSpeed;
    int leftSpeed = speed, rightSpeed = speed;
    float turningAngle = servoPosition * 5.85f / 200; // Convert servo to angle

    turningAngle *= PI;
    turningAngle /= 180.f;

    innerSpeed =
        baseSpeed *
        (1.f - settings.diffCoef * (1.50f * tanf(turningAngle)) / 2.f * 1.85f);
    outerSpeed =
        baseSpeed *
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

    tfc.setServo_i(0, servoPosition);
    tfc.setMotorPWM_i(-leftSpeed, -rightSpeed);

    data.steerData.servoPosition = servoPosition;
    data.steerData.angle = turningAngle;
    data.motorData.state = motorState;
    data.motorData.leftSpeed = leftSpeed;
    data.motorData.rightSpeed = rightSpeed;
    data.mode = settings.mode;

    sendData();
}

void Core::sendData() {
    // Line tracer
    data.cameraData.regionsListSize = tracer.size();
    data.cameraData.unchangedLeft = tracer.getUnchangedLeft();
    data.cameraData.unchangedRight = tracer.getUnchangedRight();
    data.cameraData.hasLeft = tracer.getHasLeft();
    data.cameraData.hasRight = tracer.getHasRight();
    data.timestamp = HW_TFC_TimeStamp;

    // Sensors
    std::optional<Shared::Vec3<uint16_t>> fxas_data = fxas.read();
    if (fxas_data.has_value()) {
        data.sensorData.gyro = *fxas_data;
    }
    std::optional<FXOS8700CQ::Data> fxos_data = fxos.read();

    if (fxos_data.has_value()) {
        data.sensorData.mag = fxos_data->mag;
        data.sensorData.accel = fxos_data->accel;
    }

    uint8_t *pData = reinterpret_cast<uint8_t *>(&data);
    enet.send(pData, sizeof(data));
}
