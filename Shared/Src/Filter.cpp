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

Filter::Filter() : yBuffer(4, 0), averageSum(0) {
    const float fc = 4.0f / FREQUENCY;
    const float pi = 3.14159265358979323846f;
    float fvalue = 0.f;

    for (uint32_t i = 0; i <= M; i++) {
        if (i - M / 2 == 0) {
            kernel[i] = 2.0f * pi * fc;
        } else {
            kernel[i] = sinf(2.0f * pi * fc * (i - M / 2)) / (i - M / 2);
            kernel[i] *= (0.42f - 0.5f * cosf((2.0f * pi * i) / M) +
                          0.08f * cosf((4.0f * pi * i) / M));
        }
        fvalue += kernel[i];
    }

    if (fvalue == 0)
        return;

    for (uint32_t i = 0; i <= M; i++) {
        kernel[i] /= fvalue;
    }
}

int16_t Filter::movingAverage() {
    int32_t size = xBuffer.size();

    if (size < 1) {
        return 0;
    }

    return static_cast<int16_t>(averageSum / (float)size);
}

int16_t Filter::windowedSinc() {
    float output = 0;
    int32_t i = xBuffer.size() - 1;

    if (xBuffer.size() < M) {
        return xBuffer[i];
    }

    for (uint32_t j = 0; j < M; ++j) {
        output += xBuffer[i - j] * kernel[j];
    }

    return static_cast<int16_t>(output);
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

    if (n < 1)
        return 0;

    output = singlePoleX * yBuffer[0] +
             (1.f - singlePoleX) * static_cast<float>(xBuffer[n]);

    yBuffer[0] = output;

    return static_cast<int16_t>(output);
}

int16_t Filter::recursiveFourStageLowPass() {
    int32_t n = xBuffer.size() - 1;
    float output = 0.f;

    if (n < 0)
        return 0;

    output = xBuffer[n] * fourStageA0 + yBuffer[0] * fourStageB1 +
             yBuffer[1] * fourStageB2 + yBuffer[2] * fourStageB3 +
             yBuffer[3] * fourStageB4;

    yBuffer.insert(yBuffer.begin(), output);
    if (yBuffer.size() > 4)
        yBuffer.pop_back();

    return static_cast<int16_t>(output);
}

uint16_t Filter::lowPassChebyshev2pole() {
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

uint16_t Filter::lowPassChebyshev4spole() {
    int32_t n = xBuffer.size() - 1;

    if (n <= 0) {
        return 0;
    }
    if (n < 4) {
        return xBuffer[n];
    }

    float output =
        chebyshev4A0 * xBuffer[n - 0] + chebyshev4A1 * xBuffer[n - 1] +
        chebyshev4A2 * xBuffer[n - 2] + chebyshev4A3 * xBuffer[n - 3] +
        chebyshev4A4 * xBuffer[n - 4] + chebyshev4B1 * yBuffer[0] +
        chebyshev4B2 * yBuffer[1] + chebyshev4B3 * yBuffer[2] +
        chebyshev4B4 * yBuffer[3];

    yBuffer.insert(yBuffer.begin(), output);
    if (yBuffer.size() > 4)
        yBuffer.pop_back();

    return static_cast<uint16_t>(output);
}
