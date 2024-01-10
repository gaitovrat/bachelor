/*
 * Vec.cpp
 *
 *  Created on: Dec 30, 2023
 *      Author: ratmirgaitov
 */

#include "Vec.h"

bool Vec3::operator==(const Vec3& other) const {
    return this->x == other.x && this->y == other.y && this->z == other.z;
}
