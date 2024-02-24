/*
 * Utils.h
 *
 *  Created on: Feb 24, 2024
 *      Author: gaito
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <cstdint>

template <typename T>
constexpr T BitRead(T value, uint8_t bit) {
	return (value >> bit) & 0x01;
}

template <typename T>
constexpr T BitSet(T value, uint8_t bit) {
	return value |= (1UL << bit);
}

template <typename T>
constexpr T BitClear(T value, uint8_t bit) {
	return value &= ~(1UL << bit);
}

template <typename T>
constexpr T BitWrite(T value, uint8_t bit, bool bitValue) {
	return bitValue ? BitSet(value, bit) : BitClear(value, bit);
}

#endif /* UTILS_H_ */
