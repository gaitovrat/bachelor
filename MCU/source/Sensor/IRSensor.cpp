#include "Sensor/IRSensor.h"

#include "Utils.h"

using namespace MCU;

IRSensor::IRSensor(TFC& tfc) :
		m_lineCrossBit(0),
		m_leftLineCrossTimer(0),
		m_middleLineCrossTimer(0),
		m_rightLineCrossTimer(0),
		m_tfc(tfc)
	{}

Shared::MotorState IRSensor::CheckState(Shared::MotorState motorState) {
	if (motorState != Shared::MotorState::Start) {
		return motorState;
	}

	if (m_tfc.ReadADC(anIR_1) > WHITE_IR_BOUND) {
		BitWrite(m_lineCrossBit, 0, 1);
		m_leftLineCrossTimer = MAX_CROSS_TIMER;
	} else if (m_leftLineCrossTimer > 0) {
		m_leftLineCrossTimer--;
	} else if (m_leftLineCrossTimer == 0) {
		BitWrite(m_lineCrossBit, 0, 0);
	}

	if (m_tfc.ReadADC(anIR_2) > WHITE_IR_BOUND) {
		BitWrite(m_lineCrossBit, 1, 1);
		m_middleLineCrossTimer = MAX_CROSS_TIMER;
	} else if (m_middleLineCrossTimer > 0) {
		m_middleLineCrossTimer--;
	} else if (m_middleLineCrossTimer == 0) {
		BitWrite(m_lineCrossBit, 1, 0);
	}

	if (m_tfc.ReadADC(anIR_3) > WHITE_IR_BOUND) {
		BitWrite(m_lineCrossBit, 2, 1);
		m_rightLineCrossTimer = MAX_CROSS_TIMER;
	} else if (m_rightLineCrossTimer > 0) {
		m_rightLineCrossTimer--;
	} else if (m_rightLineCrossTimer == 0) {
		BitWrite(m_lineCrossBit, 2, 0);
	}

	if (m_lineCrossBit == 0b111 || m_lineCrossBit == 0b101) {
		return Shared::MotorState::Stop;
	}

	return Shared::MotorState::Start;
}
