#ifndef DATA_H_
#define DATA_H_

#include "Vec.h"

namespace Shared {
struct SensorData {
	Vec3<uint16_t> accel;
	Vec3<uint16_t> mag;
	Vec3<uint16_t> gyro;
};
}

#endif
