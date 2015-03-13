#pragma once

#include <cfloat>
#include <cmath>

template<typename T> class Vec2 {

	friend std::ostream& operator<<(std::ostream& os, const Vec2& v) {
		os << v.x << " " << v.y;
		return os;
	}

public:

	T x;
	T y;

	inline Vec2() {
	}

	inline Vec2(const T x, const T y) {
		this->x = x; 
		this->y = y; 
	}

	inline explicit Vec2(const T t) {
		this->x = t; 
		this->y = t; 
	}

	template<typename T2> inline Vec2(const Vec2<T2>& v) {
		this->x = (T)v.x; 
		this->y = (T)v.y; 
	}

	T& operator[](const int i) {
		return *((&x) + i);
	}

	const T& operator[](const int i) const {
		return *((&x) + i);
	}

	const Vec2<T> xy() const {
		return Vec2<T>(x, y);
	}

	inline bool operator<(const Vec2<T>& v) const {
		if(x == v.x) {
			return y < v.y;
		} else {
			return x < v.x;
		}
	}

	inline float length() const {
		return sqrtf((float)(x * x + y * y));
	}

	inline float lengthSquared() const {
		return (float)(x * x + y * y);
	}

	inline void normalize() {
		const float invlen = 1.f / length();
		x = (T)(x * invlen);
		y = (T)(y * invlen);
	}

	static Vec2 normalize(const Vec2& v) {
		Vec2 result = v;
		result.normalize();
		return result;
	}

	static T length (const Vec2& v) {
		return sqrtf(v.x * v.x + v.y * v.y);
	}

	int getDominantDimension() const {
		return x > y ? 0 : 1;
	}

	static Vec2 pseudoOrthogonal(const Vec2& v) {
		return Vec2(-1 * v.y, v.x);
	}

	inline void operator+=(const Vec2& v) {
		x += v.x;
		y += v.y;
	}

	inline void operator-=(const Vec2& v) {
		x -= v.x;
		y -= v.y;
	}

	inline void operator*=(const T f) {
		x *= f;
		y *= f;
	}

	inline void operator*=(const Vec2 v) {
		x *= v.x;
		y *= v.y;
	}

	inline void operator/=(const Vec2 v) {
		x /= v.x;
		y /= v.y;
	}

	inline void operator%=(const Vec2 v) {
		x %= v.x;
		y %= v.y;
	}

	friend inline Vec2 operator-(const Vec2& v0, const Vec2& v1) {
		return Vec2(v0.x - v1.x, v0.y - v1.y);
	}

	friend inline Vec2 operator+(const Vec2& v0, const Vec2& v1) {
		return Vec2(v0.x + v1.x, v0.y + v1.y);
	}

	friend inline Vec2 operator*(T x, const Vec2& v) {
		return Vec2(x * v.x, x * v.y);
	}

	friend inline T dot(const Vec2& v0, const Vec2& v1) {
		return v0.x * v1.x + v0.y * v1.y;
	}

	friend inline Vec2 operator*(const Vec2& v0, const Vec2& v1) {
		return Vec2(v0.x * v1.x, v0.y * v1.y);
	}

	friend inline Vec2 operator%(const Vec2& v0, const Vec2& v1) {
		return Vec2(v0.x % v1.x, v0.y % v1.y);
	}

	friend inline Vec2 operator/(const Vec2& v0, const Vec2& v1) {
		return Vec2(v0.x / v1.x, v0.y / v1.y);
	}

	friend inline bool operator==(const Vec2& v0, const Vec2& v1) {
		return (v0.x == v1.x) && (v0.y == v1.y);
	}
	
	friend inline bool operator!=(const Vec2& v0, const Vec2& v1) {
		return (v0.x != v1.x) || (v0.y != v1.y);
	}
	
	friend inline Vec2 lerp(const Vec2& a, const Vec2& b, const float w) {
		return Vec2(	
			a.x + w * (b.x - a.x),
			a.y + w * (b.y - a.y));
	}

	static Vec2 min(const Vec2& v0, const Vec2& v1) {
		return Vec2(
			Tools::min(v0.x, v1.x),
			Tools::min(v0.y, v1.y));
	}

	static Vec2 max(const Vec2& v0, const Vec2& v1) {
		return Vec2(
			Tools::max(v0.x, v1.x),
			Tools::max(v0.y, v1.y));
	}
};

typedef Vec2<short> Vec2s;
typedef Vec2<unsigned short> Vec2us;
typedef Vec2<int> Vec2i;
typedef Vec2<unsigned int> Vec2ui;
typedef Vec2<float> Vec2f;

// #ifndef NO_PLEXUS_TYPE
// 
// class Vec2TypeTrait {
// 
// public:
// 
// 	typedef Vec2f Type;
// 
// 	static const char* getName() {
// 		return "Vec2";
// 	}
// };
// 
// typedef Type<Vec2TypeTrait> Vec2Type;
// 
// #endif
