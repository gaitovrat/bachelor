#include "Settings.h"

Settings::Settings(double P, double I, double D, float diffCoef, uint16_t maxSpeed, Mode rideMode)
	: PID(P, I, D), DiffCoef(diffCoef), MaxSpeed(maxSpeed), RideMode(rideMode) {}
