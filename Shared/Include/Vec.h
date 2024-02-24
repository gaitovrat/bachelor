/*
 * Vec.h
 *
 *  Created on: Nov 18, 2023
 *      Author: ratmirgaitov
 */

#ifndef VEC_H_
#define VEC_H_

#include <cstdint>

template <typename T>
union Vec3 {
    struct {
        T X, Y, Z;
    };
    T Values[3];

    Vec3(T x, T y, T z) : X(x), Y(y), Z(z) {}

    Vec3() : Vec3(0, 0, 0) {}
};

#endif /* VEC_H_ */
