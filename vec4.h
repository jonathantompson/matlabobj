#pragma once

#include "vec3.h"

template<typename T> class Vec4 {

	friend std::ostream& operator<<(std::ostream& os, const Vec4& v) {
		os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
		return os;
	}

public:

	T x;
	T y;
	T z;
	T w;

	inline Vec4() {
	};

	inline Vec4(const T x, const T y, const T z, const T w) {
		this->x = x; 
		this->y = y; 
		this->z = z;
		this->w = w;
	};

	inline explicit Vec4(const T x) {
		this->x = x; 
		this->y = x; 
		this->z = x;
		this->w = x;
	};

	template<typename T2> inline Vec4(const Vec4<T2>& v) {
		this->x = (T)v.x;
		this->y = (T)v.y;
		this->z = (T)v.z;
		this->w = (T)v.w;
	}

	template<typename T2, typename T3> inline Vec4(const Vec3<T2>& v, const T3 w) {
		this->x = (T)v.x;
		this->y = (T)v.y;
		this->z = (T)v.z;
		this->w = (T)w;
	}

#ifdef __CUDACC__
	operator float4() const {
		return make_float4(x, y, z, w);
	}

	inline Vec4(const float4& f) {
		this->x = f.x; 
		this->y = f.y; 
		this->z = f.z;
		this->w = f.w;
	}
#endif

	// To-Do: Once required, add more swizzles here
	Vec4 xyzw() const {
		return Vec4f(x, y, z, w);
	}

	Vec3<T> xyz() const {
		return Vec3<T>(x, y, z);
	}

	Vec2<T> xy() const {
		return Vec2<T>(x, y);
	}

	T& operator[](const int i) {
		if(i == 0) {
			return x;
		} else if(i == 1) {
			return y;
		} else if(i == 2) {
			return z;
		} else {
			assert(i == 3);
			return w;
		}
	}

	const T& operator[](const int i) const {
		if(i == 0) {
			return x;
		} else if(i == 1) {
			return y;
		} else if(i == 2) {
			return z;
		} else {
			assert(i == 3);
			return w;
		}
	}

	bool isDirection() const {
		return w == 0.0;
	}
	
	bool isPosition() const {
		return !isDirection();
	}

	inline void ceil() {
		x = ceilf(x);
		y = ceilf(y);
		z = ceilf(z);
		w = ceilf(w);
	};

	inline Vec4 ceiled() {
		return Vec4(ceilf(x), ceilf(y), ceilf(z), ceilf(w));
	};

	inline float length() const {
		return sqrtf(x * x + y * y + z * z + w * w);
	};

	inline float lengthSquared() const {
		return x * x + y * y + z * z + w * w;
	};

	inline void normalize() {		
		const float invlen = 1.f / length();
		x *= invlen;
		y *= invlen;
		z *= invlen;
		w *= invlen;
	};

	static Vec4 normalize(const Vec4& v) {		
		Vec4 result = v;
		result.normalize();
		return result;
	};

	friend inline float dot(const Vec4& v1, const Vec4& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

	static Vec4 min(const Vec4& v, const Vec4& v2) {
		return Vec4(	
			v.x < v2.x ? v.x : v2.x,
			v.y < v2.y ? v.y : v2.y,
			v.z < v2.z ? v.z : v2.z,
			v.w < v2.w ? v.w : v2.w);
	}

	static Vec4 max(const Vec4& v, const Vec4& v2) {
		return Vec4(	
			v.x > v2.x ? v.x : v2.x,
			v.y > v2.y ? v.y : v2.y,
			v.z > v2.z ? v.z : v2.z,
			v.w > v2.w ? v.w : v2.w);
	}	

	static Vec4 abs(const Vec4& v) {
		return Vec4(	
			v.x > 0 ? v.x : -v.x,
			v.y > 0 ? v.y : -v.y,
			v.z > 0 ? v.z : -v.z,
			v.w > 0 ? v.w : -v.w);
	}

	static Vec4 LERP(const Vec4& a, const Vec4& b, const float t) {
		return Vec4(	
			a.x + t * (b.x - a.x),
			a.y + t * (b.y - a.y),
			a.z + t * (b.z - a.z),
			a.w + t * (b.w - a.w));
	}

	inline void operator+=(const Vec4& a) {
		x += a.x;
		y += a.y;
		z += a.z;
		w += a.w;
	};

	inline void operator-=(const Vec4& a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;
	};

	inline void operator*=(const T a) {
		x *= a;
		y *= a;
		z *= a;
		w *= a;
	};

	friend inline Vec4 operator-(const Vec4& a) {
		return Vec4(-a.x, -a.y, -a.z, -a.w);
	}

	friend inline Vec4 operator+(const Vec4& a, const Vec4& b) {
		return Vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
	}

	friend inline Vec4 operator-(const Vec4& a, const Vec4& b) {
		return Vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
	}

	// Skalar mal Vektor ist definiert - nicht andersherum - streng gesehen
	// * xn-Matrizen haben m Zeilen und n Spalten
	// * Vektoren sind Spaltenvektoren. 
	// * Ein Skalar ist eine 1x1-Matrix4f - eine Zeile, eine Spalte.
	// * Ein Vektor ist eine 3x1-Matrix4f - drei Zeilen, eine Spalte.
	// * Matrix4fmultiplikation ist definiert wenn 3x1x1x1 in der Mitte "passt" und
	// * ergibt weider eine 3x1 Matrix4f, also einen Spaltenvektor.
	friend inline Vec4 operator*(const T f, const Vec4& v) {
		return Vec4(f * v.x, f * v.y, f * v.z, f * v.w);
	}

	// In some future time, we will exchange this with the scale operator
	// To safely enforce compatibility, first remove both
	friend inline T operator*(const Vec4& v1, const Vec4& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

	friend inline bool operator==(const Vec4& v1, const Vec4& v2) {
		return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
	}
};

typedef Vec4<short> Vec4s;
typedef Vec4<unsigned short> Vec4us;
typedef Vec4<int> Vec4i;
typedef Vec4<unsigned int> Vec4ui;
typedef Vec4<float> Vec4f;

// class Vec4TypeTrait {
// 
// public:
// 
// 	typedef Vec4f Type;
// 
// 	static const char* getName() {
// 		return "Vec4";
// 	}
// };
// 
// typedef Type<Vec4TypeTrait> Vec4Type;

