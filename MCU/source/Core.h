#ifndef CORE_H_
#define CORE_H_

#include "tfc.h"
#include "Enet.h"
#include "PID.h"
#include "LineTracer.h"
#include "Shared/Data.h"
#include "Shared/Settings.h"
#include "Shared/MotorState.h"
#include "Sensor/FXAS21002.h"
#include "Sensor/FXOS8700CQ.h"
#include "Sensor/IRSensor.h"

namespace MCU {
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
	Shared::Settings m_settings;
	Shared::MotorState m_motorState;
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

	Shared::Data m_data;

	// Sensors
	FXOS8700CQ m_fxos;
	FXAS21002 m_fxas;
	IRSensor m_ir;
};
}

#endif /* CORE_H_ */
