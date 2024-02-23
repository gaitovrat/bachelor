#ifndef _REGION_H
#define _REGION_H

#include <cstdint>

#include "Defines.h"

struct Region {
	uint8_t left;
	uint8_t right;
	uint8_t color;

	Region(uint8_t left = minLeft, uint8_t right = maxRight, uint8_t color =
			COLOR_WHITE) {
		if (left < minLeft) {
			left = minLeft;
		}
		if (right > maxRight) {
			right = maxRight;
		}

		this->left = left;
		this->right = right;
		this->color = color;
	}

	uint8_t getSize() const {
		return right - left;
	}

	uint8_t getCenter() const {
		return static_cast<uint8_t>((right + left) / 2);
	}

	bool isBlack() {
		return color == COLOR_BLACK;
	}
	bool isWhite() {
		return color == COLOR_WHITE;
	}

	bool operator==(const Region &rhs) {
		return this->left == rhs.left && this->right == rhs.right
				&& this->color == rhs.color;
	}

	bool operator!=(const Region &rhs) {
		return !(*this == rhs);
	}

	const static uint8_t minLeft = BLACK_COUNT;
	const static uint8_t maxRight = TFC_CAMERA_LINE_LENGTH - BLACK_COUNT - 1;
};

#endif // _REGION_H
