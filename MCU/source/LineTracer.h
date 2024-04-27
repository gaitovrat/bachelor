#ifndef _LINETRACER_H
#define _LINETRACER_H

#include <cstdint>
#include <list>
#include <utility>
#include <vector>

#include "Shared/Image.h"
#include "Shared/Region.h"

namespace MCU {
class LineTracer {
    std::vector<Shared::Region> currentRegions;
    uint64_t listSize;
    bool computedRegion;
    bool unchangedLeft;
    bool unchangedRight;
    bool hasLeft;
    bool hasRight;
    uint8_t blackRegionsCount;
    uint8_t whiteRegionsCount;
    std::pair<uint8_t, uint8_t> currentAverage;
    std::pair<uint8_t, uint8_t> currentMedian;
    std::list<Shared::Region> imageRegionList;

public:
    LineTracer(uint64_t listSize);

    void addImage(const Shared::Image &image, bool forceSearchRegions = false);

    unsigned int getLeft();

    unsigned int getRight();

    Shared::Region getLastRawRegion() const;

    bool getHasLeft() const;

    bool getHasRight() const;

    bool getUnchangedLeft() const;

    bool getUnchangedRight() const;

    Shared::Region getDistances(const Shared::Image &image,
                                bool hasPrevDistance,
                                bool forceSearchRegions = false);

    bool findByPreviousIndex(const Shared::Image &image,
                             Shared::Region &foundRegion);

    std::vector<Shared::Region> getRegions(
        const Shared::Image &image, uint8_t leftIndex = 0,
        uint8_t rightIndex = Shared::Image::LINE_LENGTH - 1,
        bool saveToClass = true);

    void reset();

    uint32_t getListSize() const;

    std::pair<uint8_t, uint8_t> getDistancesPair();
};
}  // namespace MCU

#endif  // NXPCUPRASPI_LINETRACER_H
