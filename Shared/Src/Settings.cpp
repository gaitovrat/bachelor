#include "Shared/Settings.h"

using namespace Shared;

Settings::Settings(const PIDData &pidData, const uint16_t maxSpeed,
                   const Mode mode, float diffCoef)
    : pidData(pidData), maxSpeed(maxSpeed), mode(mode), diffCoef(diffCoef) {}
