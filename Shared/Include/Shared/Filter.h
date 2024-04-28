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

    static constexpr uint32_t N = 8;
    std::vector<int16_t> xBuffer;
    std::vector<int16_t> yBuffer;

    int64_t averageSum;

public:
    /*
     * Constructor
     */
    Filter();

    /*
     * Add a new value to the filter
     * @param value the new value to add
     */
    void add(int16_t value);

    /*
     * Reset the filter
     */
    void reset();

    /*
     * Get the current value of "Moving average" filter
     * @return the current value of the filter
     */
    int16_t movingAverage();

    /*
     * Get the current value of "Single pole recursive" filter
     * @return the current value of the filter
     */
    int16_t singlePoleRecursive();

    /*
     * Get the current value of "Recursive 4 stage low pass" filter
     * @return the current value of the filter
     */
    int16_t recursiveFourStageLowPass();

    /*
     * Get the current value of "Low pass Chebysev" filter
     * @return the current value of the filter
     */
    int16_t lowPassChebyshev2pole();
};
}  // namespace Shared

#endif /* FILTER_H_ */
