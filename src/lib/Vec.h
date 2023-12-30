/*
 * Vec.h
 *
 *  Created on: Nov 18, 2023
 *      Author: ratmirgaitov
 */

#ifndef VEC_H_
#define VEC_H_

#include <cstdint>

union Vec3 {
	struct {
		int16_t X, Y, Z;
	};
	int16_t Values[3];

	bool operator==(const Vec3& other) const;
};

#endif /* VEC_H_ */
