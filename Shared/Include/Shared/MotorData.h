#ifndef INCLUDE_MOTORDATA_H_
#define INCLUDE_MOTORDATA_H_

#include "Mode.h"
#include "MotorState.h"

namespace Shared {
struct MotorData {
    MotorState state;
    int leftSpeed;
    int rightSpeed;
};
} // namespace Shared

#endif /* INCLUDE_MOTORDATA_H_ */
