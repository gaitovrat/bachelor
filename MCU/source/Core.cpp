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
    : m_settings(), m_motorState(Shared::MotorState::Stop),
      m_tracer(TRACER_HISTORY_SIZE),
      m_pidData(m_settings.P, m_settings.I, m_settings.D),
      m_pid(m_pidData, P_ON_E, DIRECT), m_ir(m_tfc) {}

void Core::Init() {
    m_tfc.InitAll();

    m_tfc.setLEDs(0b1111);
    m_enet.Init(sizeof(Shared::Data), 5000);
    m_fxos.Init();
    m_fxas.Init();

    m_tfc.MotorPWMOnOff(true);
    m_tfc.ServoOnOff(true);
    m_tfc.setPWMMax(m_settings.MaxSpeed);

    m_tfc.setLEDs(0);
}

void Core::Calibrate() {
    switch (m_tfc.getDIPSwitch() >> 1) {
    case 0b100: {
        m_tfc.MotorPWMOnOff(false);
        m_tfc.ServoOnOff(true);
        m_tfc.setServo_i(0, 0);

        int pa = m_tfc.ReadPot_i(0);
        int pb = m_tfc.ReadPot_i(1);

        m_tfc.setLEDs(LedCenter(pa) | (LedCenter(pb) << 2));
        PRINTF("DutyCycle: %4d %4d\r\n", TFC_SERVO_DEFAULT_CENTER + pa,
               TFC_SERVO_DEFAULT_CENTER + pb);

        // Workaround for diagnostic purpose only! Do not use it for control!
        // Temporarily enable full servo control
        tfc_setting_s curset = m_tfc.m_setting;
        m_tfc.m_setting.servo_center[0] = TFC_SERVO_DEFAULT_CENTER + pa;
        m_tfc.m_setting.servo_max_lr[0] = TFC_ANDATA_MINMAX;
        m_tfc.m_setting.servo_center[1] = TFC_SERVO_DEFAULT_CENTER + pb;
        m_tfc.m_setting.servo_max_lr[1] = TFC_ANDATA_MINMAX;

        m_tfc.setServo_i(0, 0);
        m_tfc.setServo_i(1, 0);

        // restore safe control
        m_tfc.m_setting = curset;
    } break;
    case 0b010: {
        m_tfc.MotorPWMOnOff(true);
        m_tfc.ServoOnOff(false);
        m_tfc.setServo_i(0, 0);

        int pa = m_tfc.ReadPot_i(0);
        int pb = m_tfc.ReadPot_i(1);
        m_tfc.setLEDs(LedCenter(pa) | (LedCenter(pb) << 2));
        PRINTF("Pot1: %5d Pot2: %5d FB-A: %6.2f FB-B: %6.2f\r\n", pa, pb,
               m_tfc.ReadFB_f(0), m_tfc.ReadFB_f(1));

        tfc_setting_s curset = m_tfc.m_setting;
        m_tfc.m_setting.pwm_max = HW_TFC_PWM_MAX;
        m_tfc.setMotorPWM_i(pa, pb);

        m_tfc.m_setting = curset;
    } break;
    case 0b001: {
        m_tfc.setServo_i(0, 0);
        m_tfc.MotorPWMOnOff(0);
        m_tfc.setLEDs(0);

        uint16_t line[TFC_CAMERA_LINE_LENGTH];

        m_tfc.getImage(0, line, TFC_CAMERA_LINE_LENGTH);

        PRINTF("Line:\r\n");
        for (uint16_t i = 0; i < TFC_CAMERA_LINE_LENGTH; i++) {
            if (i % 16 == 0)
                PRINTF("\r\n");
            PRINTF(" %03X", line[i]);
        }
        PRINTF("\r\n");
        PRINTF("%lX\r\n", m_tfc.getDIPSwitch());
    } break;
    default: {
        m_tfc.setServo_i(0, 0);
        m_tfc.MotorPWMOnOff(0);

        PRINTF("SwA: %ld SwB: %ld Pot1: %5ld Pot2: %5ld BAT: %5.2f\r\n",
               m_tfc.getPushButton(0), m_tfc.getPushButton(1),
               m_tfc.ReadPot_i(0), m_tfc.ReadPot_i(1),
               m_tfc.ReadBatteryVoltage_f());

        uint8_t bat = 0;
        if (m_tfc.getPushButton(0))
            bat |= 0x3;
        if (m_tfc.getPushButton(1))
            bat |= 0xC;
        m_tfc.setLEDs(bat);
    } break;
    }
}

void Core::Drive() {
    static uint32_t oldTick = 0;

    m_enet.Check();

    if (m_tfc.getPushButton(0) == 1) {
        if (m_motorState == Shared::MotorState::Start) {
            m_motorState = Shared::MotorState::Stop;
        } else {
            m_motorState = Shared::MotorState::Start;
        }
    }

    if (HW_TFC_TimeStamp != oldTick) {
        if (m_tfc.getDIPSwitch() & 0x01) {
            Calibrate();
            oldTick = HW_TFC_TimeStamp;
        } else {
            Update();
            oldTick = HW_TFC_TimeStamp;
        }
    }
}

void Core::Update() {
    PrintCurrentState();

    if (m_motorState == Shared::MotorState::Start) {
        m_tfc.setLEDs(0b1001);
    } else {
        m_tfc.setLEDs(0b0000);
    }

    uint16_t line[Shared::Image::LINE_LENGTH];
    m_tfc.getImage(0, line, Shared::Image::LINE_LENGTH);
    memcpy(m_data.CarCameraData.Line, line, Shared::Image::LINE_LENGTH);
    Shared::Image image(line);
    m_tracer.AddImage(image);
    m_data.CarCameraData.RegionsCount =
        m_tracer.Regions(image, 0, Shared::Image::LINE_LENGTH - 1, false)
            .size();

    auto &distances = m_tracer.DistancesPair();
    m_data.CarCameraData.LeftDistance = distances.first;
    m_data.CarCameraData.RightDistance = distances.second;

    const int leftDistance = m_data.CarCameraData.LeftDistance;
    const int rightDistance =
        Shared::Image::LINE_LENGTH - m_data.CarCameraData.RightDistance;

    const float leftRatio =
        static_cast<float>(leftDistance) / static_cast<float>(rightDistance);
    const float rightRatio =
        static_cast<float>(rightDistance) / static_cast<float>(leftDistance);

    float ratioDiff = rightRatio - leftRatio;

    if (m_motorState != Shared::MotorState::Stop) {
        m_pidData.Input = ratioDiff;
        m_pid.Compute();
        m_steerSetting = static_cast<float>(m_pidData.Output);
    } else {
        m_steerSetting = 0.f;
    }

    switch (m_motorState) {
    case Shared::MotorState::Start: {
        m_speed = 100;
        m_tfc.setPWMMax(m_settings.MaxSpeed);

        m_pid.SetMode(AUTOMATIC);
        m_servoPosition = static_cast<int16_t>(m_steerSetting);

        break;
    }
    case Shared::MotorState::Stop:
    default: {
        m_speed = 0;
        m_pid.SetMode(MANUAL);
        m_servoPosition = 0;
        m_tfc.setMotorPWM_i(0, 0);
        Reset();
        return;
    }
    }

    m_motorState = m_ir.CheckState(m_motorState);
    SetRide();
}

void Core::PrintCurrentState() {
    const char *state = nullptr;

    switch (m_motorState) {
    case Shared::MotorState::Stop:
        state = "Stop";
        break;
    case Shared::MotorState::Start:
        state = "Start";
        break;
    default:
        state = "Unknown";
    }

    PRINTF("State: %s\r\n", state);
}

void Core::Reset() {
    m_tracer.Reset();
    ResetRegulator();
}

void Core::SetRide() {
    if (m_servoPosition > 1000)
        m_servoPosition = 1000;
    if (m_servoPosition < -1000)
        m_servoPosition = -1000;

    float innerSpeed = 0.f, outerSpeed = 0.f;
    float baseSpeed = m_settings.MaxSpeed;
    int leftSpeed = m_speed, rightSpeed = m_speed;
    float turningAngle =
        m_servoPosition * 5.85f / 200; // Convert servo to angle

    turningAngle *= PI;
    turningAngle /= 180.f;

    innerSpeed =
        baseSpeed * (1.f - m_settings.DiffCoef * (1.50f * tanf(turningAngle)) /
                               2.f * 1.85f);
    outerSpeed =
        baseSpeed * (1.f + m_settings.DiffCoef * (1.50f * tanf(turningAngle)) /
                               2.f * 1.85f);

    if (!(m_tracer.UnchangedRight() && m_tracer.UnchangedLeft())) {
        if (turningAngle > 0.f) {
            rightSpeed = innerSpeed;
            leftSpeed = outerSpeed;
        } else {
            leftSpeed = innerSpeed;
            rightSpeed = outerSpeed;
        }

        leftSpeed *= 0.75f;
        rightSpeed *= 0.75f;
        m_servoPosition *= 1.5f;
    }

    m_tfc.setServo_i(0, m_servoPosition);
    m_tfc.setMotorPWM_i(-leftSpeed, -rightSpeed);

    m_data.CarSteerData.ServoPosition = m_servoPosition;
    m_data.CarSteerData.Angle = turningAngle;
    m_data.CarMotorData.CarMotorState = m_motorState;
    m_data.CarMotorData.RideMode = m_settings.RideMode;
    m_data.CarMotorData.LeftSpeed = leftSpeed;
    m_data.CarMotorData.RightSpeed = rightSpeed;

    SendData();
}

void Core::SendData() {
    // Line tracer
    m_data.CarCameraData.RegionsListSize = m_tracer.ListSize();
    m_data.CarCameraData.UnchangedLeft = m_tracer.UnchangedLeft();
    m_data.CarCameraData.UnchangedRight = m_tracer.UnchangedRight();
    m_data.CarCameraData.HasLeft = m_tracer.HasLeft();
    m_data.CarCameraData.HasRight = m_tracer.HasRight();
    m_data.Timestamp = HW_TFC_TimeStamp;

    // PID
    memcpy(&m_data.CarSteerData.SteerPIDData, &m_pidData,
           sizeof(Shared::PIDData));

    // Sensors
    std::optional<Shared::Vec3<uint16_t>> fxas_data = m_fxas.Read();
    if (fxas_data.has_value()) {
        m_data.CarSensorData.gyro = *fxas_data;
    }
    std::optional<FXOS8700CQ::Data> fxos_data = m_fxos.Read();

    if (fxos_data.has_value()) {
        m_data.CarSensorData.mag = fxos_data->mag;
        m_data.CarSensorData.accel = fxos_data->accel;
    }

    PRINTF("%d %d %d %d %d %d\r\n", sizeof(m_data),
           sizeof(m_data.CarCameraData), sizeof(m_data.CarMotorData),
           sizeof(m_data.CarSteerData), sizeof(m_data.CarSensorData),
           sizeof(m_data.Timestamp));
    uint8_t *pData = reinterpret_cast<uint8_t *>(&m_data);
    m_enet.Send(pData, sizeof(m_data));
}

void Core::ResetRegulator() {
    m_pidData.Input = 0;
    m_pidData.Output = 0;
    m_pidData.SetPoint = 0;

    m_pid = PID(m_pidData, P_ON_E, DIRECT);
}
