#ifndef DATA_H_
#define DATA_H_

#include "Vec.h"

namespace Shared {
struct SensorData {
    Vec3<int16_t> accel;
    Vec3<int16_t> mag;
    Vec3<int16_t> gyro;
};
} // namespace Shared

#endif
