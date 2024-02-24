#ifndef _REGION_H
#define _REGION_H

#include <cstdint>

#include "Image.h"

struct Region {
	static constexpr uint8_t MIN_LEFT = Image::BLACK_COUNT;
	static constexpr uint8_t MAX_RIGHT = Image::LINE_LENGTH - Image::BLACK_COUNT - 1;

	uint8_t Left;
	uint8_t Right;
	uint8_t Color;

	Region(uint8_t left = MIN_LEFT, uint8_t right = MAX_RIGHT, uint8_t color = Image::COLOR_WHITE);

	uint8_t Size() const;

	uint8_t Center() const;

	bool IsBlack();

	bool IsWhite();

	bool operator==(const Region &rhs);

	bool operator!=(const Region &rhs);
};

#endif // _REGION_H
