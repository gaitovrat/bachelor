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
    static constexpr uint16_t DEFAULT_MAX_SPEED = 1000;
    static constexpr uint16_t DEFAULT_SPEED = 100;
    static constexpr float DEFAULT_DIFF_COEF = 1.28f;

    const PIDData pidData;
    const uint16_t maxSpeed;
    const uint16_t speed;
    const Mode mode;
    const float diffCoef;

    Settings(const PIDData &pidData = PIDData(DEFAULT_ERROR, DEFAULT_INTEGRAL,
                                              DEFAULT_DERIVATIVE),
             uint16_t maxSpeed = DEFAULT_MAX_SPEED,
             uint16_t speed = DEFAULT_SPEED, Mode mode = Mode::DEFAULT,
             float diffCoef = DEFAULT_DIFF_COEF);
};
} // namespace Shared

#endif //_MODE_SETTING_H
