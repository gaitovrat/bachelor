/*
 * Vec.h
 *
 *  Created on: Nov 18, 2023
 *      Author: ratmirgaitov
 */

#ifndef VEC_H_
#define VEC_H_

#include <cstdint>

namespace Shared {
template <typename T> union Vec3 {
    struct {
        T x, y, z;
    };
    T values[3];

    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    Vec3() : Vec3(0, 0, 0) {}
};
} // namespace Shared

#endif /* VEC_H_ */
