/*
 * Filter.cpp
 *
 *  Created on: Apr 1, 2024
 *      Author: gaito
 */
#include "Filter.h"

#include <cmath>

using namespace MCU;

Filter::Filter(uint32_t bufferSize)
    : fourStageOutputs(4, .0f), chebyshev2Outputs(2, .0f),
      chebyshev4Outputs(4, .0f), N(std::max(bufferSize, M)), averageSum(0) {
    const float fc = 4.0f / FREQUENCY;
    const float pi = 3.14159265358979323846f;
    float fvalue;

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
    uint32_t size = buffer.size();

    if (size < 1)
        return 0;

    return static_cast<int16_t>(averageSum / buffer.size());
}

int16_t Filter::windowedSinc() {
    float output;

    if (buffer.size() < M) {
        return 0;
    }

    for (uint32_t i = 0; i < M; ++i) {
        output += buffer[i] * kernel[i];
    }

    return static_cast<int16_t>(output);
}

void Filter::add(int16_t value) {
    if (buffer.size() >= N) {
        averageSum -= buffer.front();
        buffer.erase(buffer.begin());
    }

    averageSum += value;
    buffer.push_back(value);
}

void Filter::reset() {
    buffer.clear();

    averageSum = 0;
}

int16_t Filter::singlePoleRecursive() {
    static float lastOutput = .0f;
    uint32_t size = buffer.size();
    float output;

    if (size < 1)
        return 0;

    output = singlePoleX * lastOutput +
             (1.f - singlePoleX) * static_cast<float>(buffer[size - 1]);

    lastOutput = output;

    return static_cast<int16_t>(output);
}

int16_t Filter::recursiveFourStageLowPass() {
    uint32_t size = buffer.size();
    float output = 0.f;

    if (size < 1) {
        return 0;
    }

    output = buffer.back() * fourStageA0 + fourStageOutputs[0] * fourStageB1 +
             fourStageOutputs[1] * fourStageB2 +
             fourStageOutputs[2] * fourStageB3 +
             fourStageOutputs[3] * fourStageB4;

    fourStageOutputs.insert(fourStageOutputs.begin(), output);
    if (fourStageOutputs.size() > 4)
        fourStageOutputs.pop_back();

    return static_cast<int16_t>(output);
}

uint16_t Filter::lowPassChebyshev2pole() {
    uint32_t size = buffer.size();
    float output = 0.f;

    if (size < 3) {
        return 0;
    }

    output += chebyshev2A0 * buffer[size - 1];
    output += chebyshev2A1 * buffer[size - 2];
    output += chebyshev2A2 * buffer[size - 3];

    output += chebyshev2B1 * chebyshev2Outputs[1];
    output += chebyshev2B2 * chebyshev2Outputs[0];

    chebyshev2Outputs[0] = chebyshev2Outputs[1];
    chebyshev2Outputs[1] = output;

    return static_cast<int16_t>(output);
}

uint16_t Filter::lowPassChebyshev4spole() {
    uint32_t size = buffer.size();

    if (size < 5) {
        return 0;
    }

    float output =
        chebyshev4A0 * buffer[size - 1] + chebyshev4A1 * buffer[size - 2] +
        chebyshev4A2 * buffer[size - 3] + chebyshev4A3 * buffer[size - 4] +
        chebyshev4A4 * buffer[size - 5] + chebyshev4B1 * chebyshev4Outputs[0] +
        chebyshev4B2 * chebyshev4Outputs[1] +
        chebyshev4B3 * chebyshev4Outputs[2] +
        chebyshev4B4 * chebyshev4Outputs[3];

    chebyshev4Outputs.insert(chebyshev4Outputs.begin(), output);
    if (chebyshev4Outputs.size() > 4)
        chebyshev4Outputs.pop_back();

    return static_cast<uint16_t>(output);
}
