//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef UDPSEND_UDPDATA_H
#define UDPSEND_UDPDATA_H

#include <cstdint>

#include "Defines.h"
#include "PIDData.h"
#include "MotorState.h"
#include "SensorData.h"
#include "Mode.h"

struct UdpData {
    // Line tracer
    uint16_t Line[TFC_CAMERA_LINE_LENGTH];
    uint32_t RegionsCount;
    size_t RegionsListSize;
    bool UnchangedLeft;
    bool UnchangedRight;
    bool HasLeft;
    bool HasRight;
    uint8_t LeftDistance;
    uint8_t RightDistance;

    // PID
    PIDData SteerData;
    PIDData MotorData;

    // Steer
    int16_t ServoPosition;
    float Angle;

    // Motor
    ::Car::MotorState MotorState;
    ::Car::Mode Mode;
    int LeftSpeed;
    int RightSpeed;

    // Sensors
    ::Car::SensorDataD SensorData;

    // Time
    uint32_t Timestamp;
};

#endif //UDPSEND_UDPDATA_H
