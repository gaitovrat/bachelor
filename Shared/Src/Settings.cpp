#include "Shared/Settings.h"

using namespace Shared;

Settings::Settings(double P, double I, double D, uint16_t maxSpeed,
                   Mode rideMode)
    : P(P), I(I), D(D), MaxSpeed(maxSpeed), RideMode(rideMode) {}
