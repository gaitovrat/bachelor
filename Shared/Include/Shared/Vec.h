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
template <typename T>
struct Vec3 {
    T x, y, z;

    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    Vec3() : Vec3(0, 0, 0) {}

    Vec3(const Vec3<T> &other) : x(other.x), y(other.y), z(other.z) {}

    Vec3(Vec3<T> &&other) : x(other.x), y(other.y), z(other.z) {}

    Shared::Vec3<T> &operator=(const Shared::Vec3<T> &other) {
        x = other.x;
        y = other.y;
        z = other.z;

        return *this;
    }
};
}  // namespace Shared

#endif /* VEC_H_ */
