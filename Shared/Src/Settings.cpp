#include "Shared/Settings.h"

using namespace Shared;

Settings::Settings(const PIDData &pidData, uint16_t maxSpeed, uint16_t speed,
                   const Mode mode, float diffCoef)
    : pidData(pidData), maxSpeed(maxSpeed), speed(speed), mode(mode),
      diffCoef(diffCoef) {}
