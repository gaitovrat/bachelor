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
template <typename T> struct Vec3 {
    T x, y, z;

    /*
     * Constructor
     * @param x the x value
     * @param y the y value
     * @param z the z value
     */
    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    /*
     * Default constructor
     */
    Vec3() : Vec3(0, 0, 0) {}

    /*
     * Copy constructor
     * @param other the other Vec3 to copy
     */
    Vec3(const Vec3<T> &other) : x(other.x), y(other.y), z(other.z) {}

    /*
     * Move constructor
     * @param other the other Vec3 to move
     */
    Vec3(Vec3<T> &&other) : x(other.x), y(other.y), z(other.z) {}

    /*
     * Copy assignment operator
     * @param other the other Vec3 to copy
     * @return the copied Vec3
     */
    Shared::Vec3<T> &operator=(const Shared::Vec3<T> &other) {
        x = other.x;
        y = other.y;
        z = other.z;

        return *this;
    }
};
} // namespace Shared

#endif /* VEC_H_ */
