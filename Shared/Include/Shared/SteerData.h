#ifndef INCLUDE_STEERDATA_H_
#define INCLUDE_STEERDATA_H_

#include <cstdint>

#include "PIDData.h"

namespace Shared {
struct SteerData {
    PIDData SteerPIDData;
    int16_t ServoPosition{};
    float Angle{};
};
} // namespace Shared

#endif /* INCLUDE_STEERDATA_H_ */
