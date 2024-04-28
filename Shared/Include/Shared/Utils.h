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
/*
 * Calculate the median of a vector
 * @param v the vector to calculate the median of
 * @return the median of the vector
 */
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

/*
 * Calculate the power of a number
 * @param base the base number
 * @param exponent the exponent
 * @return the result of the base raised to the exponent
 */ 
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
/*
 * Convert a vector of Data to a Json::Value
 * @param data the vector of Data to convert
 * @return the Json::Value
 */
Json::Value dataToJson(const std::vector<Shared::Data> &data);

/*
 * Convert a Data to a Json::Value
 * @param data the Data to convert
 * @return the Json::Value
 */
Json::Value dataToJson(const Shared::Data &data);

/*
 * Convert a Json::Value to a vector of Data
 * @param json the Json::Value to convert
 * @return the vector of Data
 */
std::vector<Shared::Data> jsonToData(const Json::Value &json);
#endif
}  // namespace Shared::Utils

#endif /* CAR_MATH_H_ */
