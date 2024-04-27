#include "Shared/Region.h"

#include <algorithm>

#include "Shared/Image.h"

using namespace Shared;

Region::Region(uint8_t left, uint8_t right, uint8_t color)
    : left(std::max(left, MIN_LEFT)),
      right(std::min(right, MAX_RIGHT)),
      color(color) {}

uint8_t Region::size() const { return this->right - this->left; }

uint8_t Region::center() const {
    return static_cast<uint8_t>((this->right + this->left) / 2);
}

bool Region::isBlack() const { return this->color == Image::COLOR_BLACK; }

bool Region::isWhite() const { return this->color == Image::COLOR_WHITE; }

bool Region::operator==(const Region &rhs) const {
    return this->left == rhs.left && this->right == rhs.right &&
           this->color == rhs.color;
}

bool Region::operator!=(const Region &rhs) const { return !(*this == rhs); }
