#ifndef INCLUDE_STEERDATA_H_
#define INCLUDE_STEERDATA_H_

#include <cstdint>

#include "PIDData.h"

namespace Shared {
struct SteerData {
    int16_t servoPosition;
    float angle;
};
} // namespace Shared

#endif /* INCLUDE_STEERDATA_H_ */
