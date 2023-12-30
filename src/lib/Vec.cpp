/*
 * Vec.cpp
 *
 *  Created on: Dec 30, 2023
 *      Author: ratmirgaitov
 */

#include "Vec.h"

bool Vec3::operator==(const Vec3& other) const {
    return this->X == other.X && this->Y == other.Y && this->Z == other.Z;
}
