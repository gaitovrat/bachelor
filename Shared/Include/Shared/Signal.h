#ifndef SIGNAL_H
#define SIGNAL_H

#include <cstdint>

namespace Shared {
struct Signal {
    enum Type {
        None,

        // Joystick signals
        INCREASE_SPEED,
        DECREASE_SPEED,
        TURN,

        // CarQt signals
        START,
        STOP
    };

    Type type;
    int16_t value;

    Signal(Type type = None, int16_t value = 0);

    virtual ~Signal() = default;
};
} // namespace Shared

#endif
