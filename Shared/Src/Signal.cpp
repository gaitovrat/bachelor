#include "Shared/Signal.h"

using namespace Shared;

Signal::Signal(const SignalType type, const int16_t value)
    : type(type), value(value) {}
