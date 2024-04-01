/*
 * Utils.h
 *
 *  Created on: Feb 24, 2024
 *      Author: gaito
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <cstdint>

namespace MCU {
template <typename T> constexpr T bitRead(T value, uint8_t bit) {
    return (value >> bit) & 0x01;
}

template <typename T> constexpr T bitSet(T value, uint8_t bit) {
    return value |= (1UL << bit);
}

template <typename T> constexpr T bitClear(T value, uint8_t bit) {
    return value &= ~(1UL << bit);
}

template <typename T>
constexpr T bitWrite(T value, uint8_t bit, bool bitValue) {
    return bitValue ? bitSet(value, bit) : bitClear(value, bit);
}

constexpr uint8_t ledCenter(const int pot) {
    if (pot < -5)
        return 0x01;

    if (pot > 5)
        return 0x2;

    return 0x3;
}

constexpr float pow(float base, int exponent) {
    float result = 1.0f;
    if (exponent > 0) {
        for (int i = 0; i < exponent; ++i) {
            result *= base;
        }
    } else {
        for (int i = 0; i < -exponent; ++i) {
            result /= base;
        }
    }
    return result;
}
} // namespace MCU

#endif /* UTILS_H_ */
