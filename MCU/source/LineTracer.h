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
    /*
     * Constructor
     * @param listSize the size of the regions list
     */
    LineTracer(uint64_t listSize);

    /*
     * Add image to the line tracer
     * @param image the image to add
     * @param forceSearchRegions force search for regions
     */
    void addImage(const Shared::Image &image, bool forceSearchRegions = false);

    /*
    * Get the distance to left boundary of the line
    * @return the left boundary of the line
    */
    unsigned int getLeft();

    /*
     * Get the distance to right boundary of the line
     * @return the right boundary of the line
     */
    unsigned int getRight();

    /*
     * Get last raw region
     * @return the last raw region
     */
    Shared::Region getLastRawRegion() const;

    /*
     * Get has left property
     * @return the has left property
     */
    bool getHasLeft() const;

    /*
     * Get has right property
     * @return the has right property
     */
    bool getHasRight() const;

    /*
    * Get unchanged left property
    * @return the unchanged left property
    */
    bool getUnchangedLeft() const;

    /*
     * Get unchanged right property
     * @return the unchanged left property
     */
    bool getUnchangedRight() const;

    /*
     * Get distances to the line boundaries
     * @param image the image to get distances from
     * @param hasPrevDistance has previous distance
     * @param forceSearchRegions force search for regions
     * @return the distances to the line boundaries
     */
    Shared::Region getDistances(const Shared::Image &image,
                                bool hasPrevDistance,
                                bool forceSearchRegions = false);

    /*
     * Find region by previous index
     * @param image the image to find the region in
     * @param foundRegion the found region
     * @return true if the region is found, false otherwise
     */
    bool findByPreviousIndex(const Shared::Image &image,
                             Shared::Region &foundRegion);

    /*
     * Get regions from the image
     * @param image the image to get regions from
     * @param leftIndex the left index of the image
     * @param rightIndex the right index of the image
     * @param saveToClass save regions to class
     * @return the regions of the image
     */
    std::vector<Shared::Region> getRegions(
        const Shared::Image &image, uint8_t leftIndex = 0,
        uint8_t rightIndex = Shared::Image::LINE_LENGTH - 1,
        bool saveToClass = true);

    /*
     * Reset the line tracer
     */
    void reset();

    /*
     * Get regions list size
     * @return the regions list size
     */
    uint32_t getListSize() const;

    /*
     * Get distances to boundaries
     * @return the distances to boundaries
     */
    std::pair<uint8_t, uint8_t> getDistancesPair();
};
}  // namespace MCU

#endif  // NXPCUPRASPI_LINETRACER_H
