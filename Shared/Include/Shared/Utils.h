/*
 * Math.h
 *
 *  Created on: Apr 10, 2023
 *      Author: Ratmir
 */

#ifndef CAR_MATH_H_
#define CAR_MATH_H_

#include <vector>
#include <cstdint>
#include <algorithm>

namespace Shared::Utils {
template<class T>
inline T Median(std::vector<T> v)
{
	size_t n = v.size() / 2;
	std::nth_element(v.begin(), v.begin() + n, v.end());

	int vn = v[n];
	if (v.size() % 2 == 1) {
		return vn;
	}
        
	std::nth_element(v.begin(), v.begin() + n - 1, v.end());
	return static_cast<T>(0.5 * (vn + v[n - 1]));
}
}

#endif /* CAR_MATH_H_ */
