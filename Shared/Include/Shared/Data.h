//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef UDPSEND_UDPDATA_H
#define UDPSEND_UDPDATA_H

#include "Image.h"
#include "Mode.h"
#include "Vec.h"

namespace Shared {
struct Data {
    // Camera data
    uint16_t line[Image::LINE_LENGTH] = {0};
    uint32_t regionsCount = 0;
    uint32_t regionsListSize = 0;
    bool unchangedLeft = false;
    bool unchangedRight = false;
    bool hasLeft = false;
    bool hasRight = false;
    uint8_t leftDistance = 0;
    uint8_t rightDistance = 0;

    // Motor data
    int32_t leftSpeed = 0;
    int32_t rightSpeed = 0;

    // Servo data
    int32_t servoPosition = 0;
    float angle = .0f;

    Vec3<int16_t> accel = {0, 0, 0};
    Vec3<int16_t> mag = {0, 0, 0};
    Vec3<int16_t> gyro = {0, 0, 0};

    uint32_t timestamp = 0;
    uint8_t mode = Mode::None;
};
} // namespace Shared

#endif // UDPSEND_UDPDATA_H
