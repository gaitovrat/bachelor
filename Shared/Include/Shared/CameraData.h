#ifndef TRACERDATA_H_
#define TRACERDATA_H_

#include <stddef.h>
#include <stdint.h>

#include "Image.h"

namespace Shared {
struct CameraData {
    uint16_t Line[Image::LINE_LENGTH];
    uint32_t RegionsCount;
    size_t RegionsListSize;
    bool UnchangedLeft;
    bool UnchangedRight;
    bool HasLeft;
    bool HasRight;
    uint8_t LeftDistance;
    uint8_t RightDistance;
};
} // namespace Shared

#endif /* TRACERDATA_H_ */
