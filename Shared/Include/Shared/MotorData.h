#ifndef INCLUDE_MOTORDATA_H_
#define INCLUDE_MOTORDATA_H_

#include "Mode.h"
#include "MotorState.h"
#include "PIDData.h"

namespace Shared {
struct MotorData {
    PIDData MotorPIDData;
    MotorState CarMotorState;
    Mode RideMode;
    int LeftSpeed;
    int RightSpeed;
};
} // namespace Shared

#endif /* INCLUDE_MOTORDATA_H_ */
