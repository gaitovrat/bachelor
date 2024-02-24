#ifndef INCLUDE_STEERDATA_H_
#define INCLUDE_STEERDATA_H_

#include <stdint.h>

#include "PIDData.h"

struct SteerData {
	PIDData SteerPIDData;
	int16_t ServoPosition;
	float Angle;
};

#endif /* INCLUDE_STEERDATA_H_ */
