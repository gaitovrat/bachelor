#ifndef _REGION_H
#define _REGION_H

#include <cstdint>

#include "Image.h"

namespace Shared {
struct Region {
    static constexpr uint8_t MIN_LEFT = Image::BLACK_COUNT;
    static constexpr uint8_t MAX_RIGHT =
        Image::LINE_LENGTH - Image::BLACK_COUNT - 1;

    uint8_t left;
    uint8_t right;
    uint8_t color;

    /*
     * Constructor
     * @param left the left boundary of the region
     * @param right the right boundary of the region
     * @param color the color of the region
     */
    Region(uint8_t left = MIN_LEFT, uint8_t right = MAX_RIGHT,
           uint8_t color = Image::COLOR_WHITE);

    /*
     * Destructor
     */
    virtual ~Region() = default;

    /*
     * Get the size of the region
     * @return the size of the region
     */
    uint8_t size() const;

    /*
     * Get the center of the region
     * @return the center of the region
     */
    uint8_t center() const;

    /*
     * Check if the region is black
     * @return true if the region is black, false otherwise
     */
    bool isBlack() const;

    /*
     * Check if the region is white
     * @return true if the region is white, false otherwise
     */
    bool isWhite() const;

    /*
     * Check if the region is equal to another region
     * @param rhs the other region
     * @return true if the regions are equal, false otherwise
     */
    bool operator==(const Region &rhs) const;

    /*
     * Check if the region is not equal to another region
     * @param rhs the other region
     * @return true if the regions are not equal, false otherwise
     */
    bool operator!=(const Region &rhs) const;
};
} // namespace Shared

#endif // _REGION_H
