#include "Shared/Settings.h"

using namespace Shared;

Settings::Settings(const PIDData &pidData, const uint16_t maxSpeed,
                   const Mode mode)
    : pidData(pidData), maxSpeed(maxSpeed), mode(mode) {}
