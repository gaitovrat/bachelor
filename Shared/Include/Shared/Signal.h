#ifndef SIGNAL_H
#define SIGNAL_H

#include <cstdint>

namespace Shared {
enum SignalType {
    None,

    // Joystick signals
    INCREASE_SPEED,
    DECREASE_SPEED,
    TURN,

    // CarQt signals
    START,
    STOP
};

struct Signal {
    SignalType type;
    int16_t value;

    Signal(SignalType type = None, int16_t value = 0);

    virtual ~Signal() = default;
};
} // namespace Shared

#endif
