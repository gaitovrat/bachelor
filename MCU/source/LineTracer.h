/*
 * LineTracer.h
 *
 *  Created on: Apr 10, 2023
 *      Author: Ratmir
 */

#ifndef LINETRACER_H_
#define LINETRACER_H_

#include <list>
#include <stddef.h>
#include <utility>
#include <vector>

#include "Shared/Image.h"
#include "Shared/Region.h"

namespace MCU {
class LineTracer {
    std::vector<Shared::Region> currentRegions;
    uint32_t listSize;
    bool computedRegion;
    bool unchangedLeft;
    bool unchangedRight;
    uint8_t blackRegionsCount;
    uint8_t whiteRegionsCount;
    bool hasLeft;
    bool hasRight;
    std::pair<uint8_t, uint8_t> currentAverage;
    std::pair<uint8_t, uint8_t> currentMedian;
    std::list<Shared::Region> imageRegionList;

  public:
    LineTracer(uint32_t listSize);

    void addImage(const Shared::Image &image, bool forceSearchRegions = false);

    unsigned int left();

    unsigned int right();

    Shared::Region lastRawRegion();

    bool getHasLeft();

    bool getHasRight();

    bool getUnchangedLeft() const;
    bool getUnchangedRight() const;

    Shared::Region distances(const Shared::Image &image, bool hasPrevDistance,
                             bool forceSearchRegions = false);

    bool findByPreviousIndex(const Shared::Image &image,
                             Shared::Region &foundRegion);

    std::vector<Shared::Region>
    regions(const Shared::Image &image, uint8_t leftIndex = 0,
            uint8_t rightIndex = Shared::Image::LINE_LENGTH - 1,
            bool saveToClass = true);

    void reset();

    size_t size();

    std::pair<uint8_t, uint8_t> &distancesPair();
};
} // namespace MCU
#endif /* LINETRACER_H_ */
