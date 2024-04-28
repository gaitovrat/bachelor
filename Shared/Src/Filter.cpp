/*
 * Filter.cpp
 *
 *  Created on: Apr 1, 2024
 *      Author: gaito
 */
#include "Shared/Filter.h"

#include <cmath>
#include <iostream>

using namespace Shared;

Filter::Filter() : yBuffer(4, 0), averageSum(0) {}

int16_t Filter::movingAverage() {
    int32_t size = xBuffer.size();

    if (size < 1) {
        return 0;
    }

    return static_cast<int16_t>(averageSum / (float)size);
}

void Filter::add(int16_t value) {
    if (xBuffer.size() >= N) {
        averageSum -= xBuffer.front();
        xBuffer.erase(xBuffer.begin());
    }

    averageSum += value;
    xBuffer.push_back(value);
}

void Filter::reset() {
    xBuffer.clear();

    averageSum = 0;
}

int16_t Filter::singlePoleRecursive() {
    int32_t n = xBuffer.size() - 1;
    float output;

    if (n < 1) return 0;

    output = singlePoleX * yBuffer[0] +
             (1.f - singlePoleX) * static_cast<float>(xBuffer[n]);

    yBuffer[0] = output;

    return static_cast<int16_t>(output);
}

int16_t Filter::recursiveFourStageLowPass() {
    int32_t n = xBuffer.size() - 1;
    float output = 0.f;

    if (n < 0) return 0;

    output = xBuffer[n] * fourStageA0 + yBuffer[0] * fourStageB1 +
             yBuffer[1] * fourStageB2 + yBuffer[2] * fourStageB3 +
             yBuffer[3] * fourStageB4;

    yBuffer.insert(yBuffer.begin(), output);
    if (yBuffer.size() > 4) yBuffer.pop_back();

    return static_cast<int16_t>(output);
}

int16_t Filter::lowPassChebyshev2pole() {
    int32_t n = xBuffer.size() - 1;
    float output = 0.f;

    if (n <= 0) {
        return 0;
    }
    if (n < 2) {
        return xBuffer[n];
    }

    output += chebyshev2A0 * xBuffer[n - 0];
    output += chebyshev2A1 * xBuffer[n - 1];
    output += chebyshev2A2 * xBuffer[n - 2];

    output += chebyshev2B1 * yBuffer[1];
    output += chebyshev2B2 * yBuffer[0];

    yBuffer[0] = yBuffer[1];
    yBuffer[1] = output;

    return static_cast<int16_t>(output);
}
