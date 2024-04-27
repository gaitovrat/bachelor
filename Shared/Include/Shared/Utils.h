/*
 * Math.h
 *
 *  Created on: Apr 10, 2023
 *      Author: Ratmir
 */

#ifndef CAR_MATH_H_
#define CAR_MATH_H_

#include <algorithm>
#include <cstdint>
#include <vector>

#ifdef PC
#include <json/json.h>

#include "Shared/Data.h"
#endif

namespace Shared::Utils {
template <class T>
inline T median(std::vector<T> v) {
    size_t n = v.size() / 2;
    std::nth_element(v.begin(), v.begin() + n, v.end());

    int vn = v[n];
    if (v.size() % 2 == 1) {
        return vn;
    }

    std::nth_element(v.begin(), v.begin() + n - 1, v.end());
    return static_cast<T>(0.5 * (vn + v[n - 1]));
}
constexpr float pow(const float base, const int exponent) {
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

#ifdef PC
Json::Value dataToJson(const std::vector<Shared::Data> &data);

Json::Value dataToJson(const Shared::Data &data);

std::vector<Shared::Data> jsonToData(const Json::Value &json);
#endif
}  // namespace Shared::Utils

#endif /* CAR_MATH_H_ */
