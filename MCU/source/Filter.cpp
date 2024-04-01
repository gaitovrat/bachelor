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
    : N(std::max(bufferSize, M)), averageSum(0) {
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
    static constexpr float alpha = 0.85f;

    uint32_t size = buffer.size();
    float output, input;

    if (size < 1)
        return 0;

    if (size < 2) {
        return buffer.back();
    }

    input = static_cast<float>(buffer[size - 1]);

    output =
        alpha * input + (1.f - alpha) * static_cast<float>(buffer[size - 2]);

    return static_cast<int16_t>(output);
}

int16_t Filter::recursiveFourStageLowPass() {
    static constexpr float a0 = 0.60f;
    static constexpr float b1 = 4.0f * a0;
    static const float b2 = -6.0f * powf(a0, 2.0f);
    static const float b3 = 4.0f * powf(a0, 3.0f);
    static const float b4 = -1.0f * powf(a0, 4.0f);
    static std::vector<float> pastOutputs(4, .0f);

    uint32_t size = buffer.size();
    float output = 0.f;

    if (size < 1) {
        return 0;
    }

    output = buffer.back() * a0 + pastOutputs[0] * b1 + pastOutputs[1] * b2 +
             pastOutputs[2] * b3 + pastOutputs[3] * b4;

    pastOutputs.insert(pastOutputs.begin(), output);
    if (pastOutputs.size() > 4)
        pastOutputs.pop_back();

    return static_cast<int16_t>(output);
}

uint16_t Filter::lowPassChebyshev2pole() {
    static constexpr float a0 = 3.869430E-02f;
    static constexpr float a1 = 7.738860E-02f;
    static constexpr float a2 = 3.869430E-02f;
    static constexpr float b1 = 1.392667E+00f;
    static constexpr float b2 = -5.474446E-01;
    static std::vector<float> pastOutputs(2, 0.0f);

    uint32_t size = buffer.size();
    float output = 0.f;

    if (size < 3) {
        return 0;
    }

    output += a0 * buffer[size - 1];
    output += a1 * buffer[size - 2];
    output += a2 * buffer[size - 3];

    output += b1 * pastOutputs[1];
    output += b2 * pastOutputs[0];

    pastOutputs[0] = pastOutputs[1];
    pastOutputs[1] = output;

    return static_cast<int16_t>(output);
}

uint16_t Filter::lowPassChebyshev4spole() {
    static constexpr float a0 = 9.726342E-04f;
    static constexpr float a1 = 3.890537E-03f;
    static constexpr float a2 = 5.835806E-03f;
    static constexpr float a3 = 3.890537E-03f;
    static constexpr float a4 = 9.726342E-04f;
    static constexpr float b1 = 3.103944E+00f;
    static constexpr float b2 = -3.774453E+00f;
    static constexpr float b3 = 2.111238E+00f;
    static constexpr float b4 = -4.562908E-01f;

    static std::vector<float> pastOutputs(4, 0.0f);
    uint32_t size = buffer.size();

    if (size < 5) {
        return 0;
    }

    float output =
        a0 * buffer[size - 1] + a1 * buffer[size - 2] + a2 * buffer[size - 3] +
        a3 * buffer[size - 4] + a4 * buffer[size - 5] + b1 * pastOutputs[0] +
        b2 * pastOutputs[1] + b3 * pastOutputs[2] + b4 * pastOutputs[3];

    pastOutputs.insert(pastOutputs.begin(), output);
    if (pastOutputs.size() > 4)
        pastOutputs.pop_back();

    return static_cast<uint16_t>(output);
}
