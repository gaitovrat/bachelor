#ifndef TRACERDATA_H_
#define TRACERDATA_H_

#include <stddef.h>
#include <stdint.h>

#include "Image.h"

namespace Shared {
struct CameraData {
    uint16_t line[Image::LINE_LENGTH];
    uint32_t regionsCount;
    uint32_t regionsListSize;
    bool unchangedLeft;
    bool unchangedRight;
    bool hasLeft;
    bool hasRight;
    uint8_t leftDistance;
    uint8_t rightDistance;
};
} // namespace Shared

#endif /* TRACERDATA_H_ */
