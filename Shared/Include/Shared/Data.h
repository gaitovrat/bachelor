//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef UDPSEND_UDPDATA_H
#define UDPSEND_UDPDATA_H

#include "CameraData.h"
#include "MotorData.h"
#include "SensorData.h"
#include "SteerData.h"

namespace Shared {
struct Data {
    CameraData cameraData;
    MotorData motorData;
    SteerData steerData;
    SensorData sensorData;
    Mode mode;
    uint64_t timestamp;
};
} // namespace Shared

#endif // UDPSEND_UDPDATA_H
