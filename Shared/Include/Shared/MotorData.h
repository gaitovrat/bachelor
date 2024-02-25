#ifndef INCLUDE_MOTORDATA_H_
#define INCLUDE_MOTORDATA_H_

#include "PIDData.h"
#include "MotorState.h"
#include "Mode.h"

namespace Shared {
struct MotorData {
	PIDData MotorPIDData;
	MotorState CarMotorState;
	Mode RideMode;
	int LeftSpeed;
	int RightSpeed;
};
}

#endif /* INCLUDE_MOTORDATA_H_ */
