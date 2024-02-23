//
// Created by zvone on 24-Apr-19.
//

#ifndef _MODE_SETTING_H
#define _MODE_SETTING_H

#include "Mode.h"
#include <cstdint>

#include "Defines.h"
#include "PIDData.h"

struct Settings {
	Settings(double P, double I, double D,
			float diffCoef, uint16_t maxSpeed, ::Car::Mode mode) :
			PIDdata{},
			DiffCoef(diffCoef),
			MaxSpeed(maxSpeed), Mode(mode) {
		PIDdata.P = P;
		PIDdata.I = I;
		PIDdata.D = D;
	}

	bool operator==(const Settings &rhs) const {
		return this->PIDdata.P == rhs.PIDdata.P
				&& this->PIDdata.I == rhs.PIDdata.I
				&& this->PIDdata.D == rhs.PIDdata.D
				&& this->DiffCoef == rhs.DiffCoef && this->Mode == rhs.Mode;
	}

	bool operator!=(const Settings &rhs) const {
		return !(*this == rhs);
	}

	PIDData PIDdata;
	float DiffCoef;
	uint16_t MaxSpeed;
	::Car::Mode Mode;
};

static const Settings RideDefaultSettings(CONST_ERROR,             // PID error
		CONST_INTEGRAL,          // PID integral
		CONST_DERIVATIVE,        // PID Derivation
		TURN_CONTROL_COEFICIENT, // DIFF coefficient
		TFC_PWM_DEFAULT_MAX,     // PWM base speed
		::Car::Mode::RideDefault);             // Ride mode


#endif //_MODE_SETTING_H
