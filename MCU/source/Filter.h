/*
 * Filter.h
 *
 *  Created on: Apr 1, 2024
 *      Author: gaito
 */

#ifndef FILTER_H_
#define FILTER_H_

#include <cmath>
#include <cstdint>
#include <vector>

#define FREQUENCY 400

namespace MCU {
class Filter {
#if (FREQUENCY / 2) % 2 == 0
    float kernel[(FREQUENCY / 2) + 1];
    static constexpr uint32_t M = FREQUENCY / 2;
#else
    float kernel[(FREQUENCY / 2)];
    constexpr uint32_t M = (FREQUENCY / 2) - 1;
#endif

    const uint32_t N;
    std::vector<int16_t> buffer;

    int averageSum;

  public:
    Filter(uint32_t bufferSize);

    void add(int16_t value);

    void reset();

    int16_t movingAverage();

    int16_t windowedSinc();

    int16_t singlePoleRecursive();

    int16_t recursiveFourStageLowPass();

    uint16_t lowPassChebyshev2pole();

    uint16_t lowPassChebyshev4spole();
};
} // namespace MCU

#endif /* FILTER_H_ */
