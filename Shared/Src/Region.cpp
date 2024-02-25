#include "Shared/Region.h"

#include <algorithm>

#include "Shared/Image.h"

using namespace Shared;

Region::Region(uint8_t left, uint8_t right, uint8_t color) :
	Left(std::max(left, MIN_LEFT)), Right(std::min(right, MAX_RIGHT)), Color(color) {}

uint8_t Region::Size() const {
	return Right - Left;
}

uint8_t Region::Center() const {
	return static_cast<uint8_t>((Right + Left) / 2);
}

bool Region::IsBlack() {
	return Color == Image::COLOR_BLACK;
}
bool Region::IsWhite() {
	return Color == Image::COLOR_WHITE;
}

bool Region::operator==(const Region &rhs) {
	return this->Left == rhs.Left && this->Right == rhs.Right
			&& this->Color == rhs.Color;
}

bool Region::operator!=(const Region &rhs) {
	return !(*this == rhs);
}
