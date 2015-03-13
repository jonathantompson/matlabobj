#pragma once

#include <cassert>
#include <cfloat>
#include <cmath>

#include "vec2.h"

template<typename T> class Vec3 {

	friend std::ostream& operator<<(std::ostream& os, const Vec3& v) {
		os << v.x << " " << v.y << " " << v.z;
		return os;
	}

public:

#pragma pack(push, 16)
	struct {
		T x;
		T y;
		T z;
		T _dumy;
	};
#pragma pack(pop)

	inline Vec3() {
	}

	inline Vec3(const T x, const T y, const T z) {
		this->x = x; 
		this->y = y; 
		this->z = z;
	}

	inline explicit Vec3(const T t) {
		this->x = t; 
		this->y = t; 
		this->z = t;
	}

	template<typename T2> inline Vec3(const Vec3<T2>& v) {
		this->x = (T)v.x;
		this->y = (T)v.y;
		this->z = (T)v.z;
	}

#ifdef __CUDACC__
	operator float3() const {
		return make_float3(x, y, z);
	}

	inline Vec3(const float3& f) {
		this->x = f.x; 
		this->y = f.y; 
		this->z = f.z;
	}
#endif

	T& operator[](const int i) {
		if(i == 0) {
			return x;
		} else if(i == 1) {
			return y;
		} else {
			assert(i == 2);
			return z;
		}
	}

	const T& operator[](const int i) const {
		if(i == 0) {
			return x;
		} else if(i == 1) {
			return y;
		} else {
			assert(i == 2);
			return z;
		}
	}

	const Vec2<T> xy() const {
		return Vec2<T>(x, y);
	}

	inline void ceil() {
		x = ceilf(x);
		y = ceilf(y);
		z = ceilf(z);
	}

	inline float length() const {
		return sqrtf(x * x + y * y + z * z);
	}

	inline float lengthSquared() const {
		return x * x + y * y + z * z;
	}

	inline void normalize() {		
		const float invlen = 1.f / length();
		x *= invlen;
		y *= invlen;
		z *= invlen;	
	}

	friend inline Vec3 abs(const Vec3& v) {
		return Vec3(	
			v.x > 0 ? v.x : -v.x,
			v.y > 0 ? v.y : -v.y,
			v.z > 0 ? v.z : -v.z);
	}

	friend inline Vec3 lerp(const Vec3& a, const Vec3& b, const float w) {
		return Vec3(	
			a.x + w * (b.x - a.x),
			a.y + w * (b.y - a.y),
			a.z + w * (b.z - a.z));
	}

	friend inline T trilerp(
		const Vec3 weight, 
		const T& value000,
		const T& value001,
		const T& value010,
		const T& value011,
		const T& value100,
		const T& value101,
		const T& value110,
		const T& value111)
	{
		const T value00 = lerp(value000, value001, weight.x);
		const T value01 = lerp(value010, value011, weight.x);
		const T value10 = lerp(value100, value101, weight.x);
		const T value11 = lerp(value110, value111, weight.x);
		const T value0 = lerp(value00, value01, weight.y);
		const T value1 = lerp(value10, value11, weight.y);
		const T value = lerp(value0, value1, weight.z);
		return value;
	}

	int getDominantDimension() const {
		if(x > y) {
			if(x > z) {
				return 0;
			}
			return 2;
		}
		if(y > z) {
			return 1;
		}
		return 2;
	}

	inline void operator+=(const Vec3& a) {
		x += a.x;
		y += a.y;
		z += a.z;
	}

	inline void operator-=(const Vec3& a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
	}

	inline void operator*=(const T a) {
		x *= a;
		y *= a;
		z *= a;
	}

	inline void operator*=(const Vec3& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}

	inline void operator/=(const Vec3& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
	}

	bool operator<(const Vec3& v) const {
		if(v.x == x) {
			if(v.y == y) {
				return v.z < z;
			} else {
				return v.y < y;
			}
		} else {
			return v.x < x;
		}
	}

	static Vec3 unitSphericalToCarthesian(const Vec2f& spherical) {
		const float phi = spherical.x;
		const float theta = spherical.y;
		const T x = sinf(phi) * sinf(theta);
		const T y = cosf(theta);
		const T z = cosf(phi) * sinf(theta);
		return Vec3(x, y, z);
	}

	static Vec2f unitCarthesianToSpherical(const Vec3& carthesian) {
#ifdef __CUDACC__
		// This is a hack to make it compile under CUDACC
		assert(false);
		return Vec2f(0, 0);
#else
		const float x = atan2f(carthesian.x, carthesian.z) + (float)M_PI;
		const float y = acosf(carthesian.y);
		return Vec2f(x, y);
#endif
	}

	friend inline Vec3 operator-(const Vec3& a) {
		return Vec3(-a.x, -a.y, -a.z);
	}

	friend inline Vec3 operator+(const Vec3& a, const Vec3& b) {
		return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	friend inline Vec3 operator-(const Vec3& a, const Vec3& b) {
		return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
	}


	// Skalar mal Vektor ist definiert - nicht andersherum - streng gesehen
	// * xn-Matrizen haben m Zeilen und n Spalten
	// * Vektoren sind Spaltenvektoren. 
	// * Ein Skalar ist eine 1x1-Matrix4f - eine Zeile, eine Spalte.
	// * Ein Vektor ist eine 3x1-Matrix4f - drei Zeilen, eine Spalte.
	// * Matrix4fmultiplikation ist definiert wenn 3x1x1x1 in der Mitte "passt" und
	// * ergibt weider eine 3x1 Matrix4f, also einen Spaltenvektor.
	friend inline Vec3 operator*(const T f, const Vec3& v) {
		return Vec3(f * v.x, f * v.y, f * v.z);
	}

	friend inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
		return Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}

	friend inline Vec3 operator/(const Vec3& v1, const Vec3& v2) {
		return Vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
	}

	friend inline Vec3 operator%(const Vec3& v1, const Vec3& v2) {
		return Vec3(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z);
	}

	friend inline Vec3 normalize(const Vec3& v) {		
		Vec3 result = v;
		result.normalize();
		return result;
	}

	friend inline float dot(const Vec3& v1, const Vec3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	friend inline float length(const Vec3& v) {
		return v.length();
	}

	friend inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
		return Vec3(	
			v1.y * v2.z - v1.z * v2.y, 
			v1.z * v2.x - v1.x * v2.z, 
			v1.x * v2.y - v1.y * v2.x);
	}

	friend Vec3 ceil(const Vec3& v) {
		return Vec3(ceilf(v.x), ceilf(v.y), ceilf(v.z));
	}

	friend inline bool operator==(const Vec3& v1, const Vec3& v2) {
		return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
	}

	friend inline bool operator!=(const Vec3& v1, const Vec3& v2) {
		return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z);
	}

	friend inline Vec3 reflect(const Vec3& delta, const Vec3& normal) { 		
		return (2.0f * dot(normal, delta)) * normal - delta;
	}

	static Vec3 min(const Vec3& v0, const Vec3& v1) {
		return Vec3(
			Tools::min(v0.x, v1.x),
			Tools::min(v0.y, v1.y),
			Tools::min(v0.z, v1.z));
	}

	static Vec3 max(const Vec3& v0, const Vec3& v1) {
		return Vec3(
			Tools::max(v0.x, v1.x),
			Tools::max(v0.y, v1.y),
			Tools::max(v0.z, v1.z));
	}
};

typedef Vec3<short> Vec3s;
typedef Vec3<unsigned short> Vec3us;
typedef Vec3<int> Vec3i;
typedef Vec3<unsigned int> Vec3ui;
typedef Vec3<float> Vec3f;

// class Vec3TypeTrait {
// 
// public:
// 
// 	typedef Vec3f Type;
// 
// 	static const char* getName() {
// 		return "Vec3";
// 	}
// };
// 
// typedef Type<Vec3TypeTrait> Vec3Type;
