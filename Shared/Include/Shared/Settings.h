//
// Created by zvone on 24-Apr-19.
//

#ifndef _MODE_SETTING_H
#define _MODE_SETTING_H

#include <stdint.h>

#include "Mode.h"
#include "PIDData.h"

namespace Shared {
struct Settings {
    static constexpr float DEFAULT_ERROR = 160.6f;
    static constexpr float DEFAULT_DERIVATIVE = 8.3f;
    static constexpr float DEFAULT_INTEGRAL = 0.5f;
    static constexpr uint32_t DEFAULT_MAX_SPEED = 1000;

    double P;
    double I;
    double D;
    float DiffCoef;
    uint16_t MaxSpeed;
    Mode RideMode;

    Settings(double P = DEFAULT_ERROR, double I = DEFAULT_INTEGRAL,
             double D = DEFAULT_DERIVATIVE,
             uint16_t maxSpeed = DEFAULT_MAX_SPEED,
             Mode rideMode = Mode::RideDefault);
};
} // namespace Shared

#endif //_MODE_SETTING_H
