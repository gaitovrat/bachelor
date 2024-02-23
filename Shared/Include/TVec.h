#ifndef VEC_H_
#define VEC_H_

#include <cstdint>

template<typename T>
union Vec2 {
	struct {
		T x, y;
	};
	T val[2];

	Vec2<T> operator-(const Vec2<T> &rhs) {
		T x = this->x - rhs.x;
		T y = this->y - rhs.y;

		return {x, y};
	}

	Vec2<T> operator+(const Vec2<T> &rhs) {
		return {this->x + rhs.x,this->y + rhs.y};
	}

	bool operator==(const Vec2<T> &rhs) {
		return this->x == rhs.x && this->y == rhs.y;
	}

	bool operator!=(const Vec2<T> &rhs) {
		return !(*this == rhs);
	}
};

template<typename T>
union Vec3 {
	struct {
		T x, y, z;
	};
	T val[3];

	Vec3<T> operator-(const Vec3<T> &rhs) {
		T x = this->x - rhs.x;
		T y = this->y - rhs.y;
		T z = this->z - rhs.z;

		return {x, y, z};
	}

	Vec3<T> operator+(const Vec3<T> &rhs) {
		return {this->x + rhs.x,this->y + rhs.y,this->z + rhs.z};
	}

	bool operator==(const Vec3<T> &rhs) {
		return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
	}

	bool operator!=(const Vec3<T> &rhs) {
		return !(*this == rhs);
	}
};

using Vec3d = Vec3<uint16_t>;
using Vec3f = Vec3<double>;
using Vec2d = Vec2<uint16_t>;
using Vec2f = Vec2<double>;


#endif /* VEC_H_ */
