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

    Region(uint8_t left = MIN_LEFT, uint8_t right = MAX_RIGHT,
           uint8_t color = Image::COLOR_WHITE);

    virtual ~Region() = default;

    uint8_t size() const;

    uint8_t center() const;

    bool isBlack() const;

    bool isWhite() const;

    bool operator==(const Region &rhs) const;

    bool operator!=(const Region &rhs) const;
};
} // namespace Shared

#endif // _REGION_H
