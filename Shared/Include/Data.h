//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef UDPSEND_UDPDATA_H
#define UDPSEND_UDPDATA_H

#include "CameraData.h"
#include "SensorData.h"
#include "MotorData.h"
#include "SteerData.h"

struct Data {
	CameraData CarCameraData;
	MotorData CarMotorData;
	SteerData CarSteerData;
    SensorData CarSensorData;
    uint64_t Timestamp;
};

#endif //UDPSEND_UDPDATA_H
