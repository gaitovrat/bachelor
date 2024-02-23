#ifndef IMUDATA_H_
#define IMUDATA_H_

#include "Vec.h"

namespace Car {
template<class T>
struct SensorData {
    T GyroData = {0};
    T MagData = {0};
    T AccelData = {0};
};

using SensorDataD = SensorData<Vec3d>;
using SensorDataF = SensorData<Vec3f>;
}

#endif // IMUDATA_H_
