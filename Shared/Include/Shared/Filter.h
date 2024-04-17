/*
 * Filter.h
 *
 *  Created on: Apr 1, 2024
 *      Author: gaito
 */

#ifndef FILTER_H_
#define FILTER_H_

#include <cstdint>
#include <vector>

#include "Shared/Utils.h"

#define FREQUENCY 50

namespace Shared {
class Filter {
    static constexpr float singlePoleX = 0.85f;

    // Four stage coefficients
    static constexpr float fourStageX = 0.6f;
    static constexpr float fourStageA0 = Utils::pow(1 - fourStageX, 4);
    static constexpr float fourStageB1 = 4.0f * fourStageX;
    static constexpr float fourStageB2 = -6.0f * Utils::pow(fourStageX, 2);
    static constexpr float fourStageB3 = 4.0f * Utils::pow(fourStageX, 3);
    static constexpr float fourStageB4 = -1.0f * Utils::pow(fourStageX, 4);

    // Chebyshev 2 coefficients
    static constexpr float chebyshev2A0 = 3.869430E-02f;
    static constexpr float chebyshev2A1 = 7.738860E-02f;
    static constexpr float chebyshev2A2 = 3.869430E-02f;
    static constexpr float chebyshev2B1 = 1.392667E+00f;
    static constexpr float chebyshev2B2 = -5.474446E-01;

    // Chebyshev 4 coefficients
    static constexpr float chebyshev4A0 = 9.726342E-04f;
    static constexpr float chebyshev4A1 = 3.890537E-03f;
    static constexpr float chebyshev4A2 = 5.835806E-03f;
    static constexpr float chebyshev4A3 = 3.890537E-03f;
    static constexpr float chebyshev4A4 = 9.726342E-04f;
    static constexpr float chebyshev4B1 = 3.103944E+00f;
    static constexpr float chebyshev4B2 = -3.774453E+00f;
    static constexpr float chebyshev4B3 = 2.111238E+00f;
    static constexpr float chebyshev4B4 = -4.562908E-01f;

#if (FREQUENCY / 2) % 2 == 0
    float kernel[(FREQUENCY / 2) + 1];
    static constexpr uint32_t M = FREQUENCY / 2;
#else
    float kernel[(FREQUENCY / 2)];
    static constexpr uint32_t M = (FREQUENCY / 2) - 1;
#endif

    static constexpr uint32_t N = 8;
    std::vector<int16_t> xBuffer;
    std::vector<int16_t> yBuffer;

    int64_t averageSum;

  public:
    Filter();

    void add(int16_t value);

    void reset();

    int16_t movingAverage();

    int16_t windowedSinc();

    int16_t singlePoleRecursive();

    int16_t recursiveFourStageLowPass();

    uint16_t lowPassChebyshev2pole();

    uint16_t lowPassChebyshev4spole();
};
} // namespace Shared

#endif /* FILTER_H_ */
