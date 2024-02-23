#ifndef CORE_H_
#define CORE_H_

#include "tfc/tfc.h"
#include "enet/Enet.h"
#include "MotorState.h"
#include "LineTracer.h"
#include "PID.h"
#include "Settings.h"
#include "UdpData.h"
#include "sensor/AccelSensor.h"
#include "sensor/MagSensor.h"

class Core {
public:
	Core();
	virtual ~Core() = default;

	void Init();

	void Calibrate();

	void Drive();

private:
	void Update();

	void PrintCurrentState();

	void Reset();

	void SetRide();

	void ResetRegulator();

	void SendData();

private:
	TFC m_tfc;
	Enet m_enet;
	Settings m_settings;
	::Car::MotorState m_motorState;
	LineTracer m_tracer;
	PID m_pid;

	// PID
	float m_steerSetting;
	int m_speed;
	// Servo
	int16_t m_prevServoPosition;
	int16_t m_servoPosition;
	uint8_t m_left;
	uint8_t m_right;

	UdpData m_data;

	// Sensors
	AccelSensor m_accelSensor;
	MagSensor m_magSensor;
};

#endif /* CORE_H_ */
