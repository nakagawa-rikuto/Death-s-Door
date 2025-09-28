#define _USE_MATH_DEFINES
#include <cmath>

#include "Vector3.h"

///-------------------------------------------/// 
/// 加法
///-------------------------------------------///
// Vector3・float（+）
Vector3 Vector3::operator+(const float& v) { return Vector3(x + v, y + v, z + v); }
// Vector3・float（+=）
Vector3& Vector3::operator+=(const float& v) {
	x += v;
	y += v;
	z += v;
	return *this;
}
// Vector3・Vector3（+）
Vector3 Vector3::operator+(const Vector3& v) { return Vector3(x + v.x, y + v.y, z + v.z); }
// Vector3・Vector3（+=）
Vector3& Vector3::operator+=(const Vector3& v) {
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
// const Vector3・float（+）
Vector3 Vector3::operator+(const float& v) const { return Vector3(x + v, y + v, z + v); }
// const Vector3・Vector3（+）
Vector3 Vector3::operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }


///-------------------------------------------/// 
/// 減法
///-------------------------------------------///
// Vector3・float（-）
Vector3 Vector3::operator-(const float& v) { return Vector3(x - v, y - v, z - v); }
// Vector3・float（-=）
Vector3& Vector3::operator-=(const float& v) {
	x -= v;
	y -= v;
	z -= v;
	return *this;
}
// Vector3・Vector3
Vector3 Vector3::operator-(const Vector3& v) { return Vector3(x - v.x, y - v.y, z - v.z); }
// Vector3・Vector3（-=）
Vector3& Vector3::operator-=(const Vector3& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}
// const Vector3・float（-）
Vector3 Vector3::operator-(const float& v) const{ return Vector3(x - v, y - v, z - v); }
// const Vector3・Vector3（-）
Vector3 Vector3::operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }


///-------------------------------------------/// 
/// 積
///-------------------------------------------///
// Vector3・float（*）
Vector3 Vector3::operator*(const float& v) { return Vector3(x * v, y * v, z * v); }
// Vector3・float（*=）
Vector3& Vector3::operator*=(const float& v) {
	x *= v;
	y *= v;
	z *= v;
	return *this;
}
// Vector3・Vector3（*）
Vector3 Vector3::operator*(const Vector3& v) { return Vector3(x * v.x, y * v.y, z * v.z); }
// Vector3・Vector3（*=）
Vector3& Vector3::operator*=(const Vector3& v) {
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}
// const Vector3・float（*）
Vector3 Vector3::operator*(const float& v) const { return Vector3(x * v, y * v, z * v); }
// const Vector3・Vector3（*）
Vector3 Vector3::operator*(const Vector3& v) const { return Vector3(x * v.x, y * v.y, z * v.z); }


///-------------------------------------------/// 
/// 除法
///-------------------------------------------///
// Vector3・float（/）
Vector3 Vector3::operator/(const float& v) { return Vector3(x / v, y / v, z / v); }
// Vector3・float（/=）
Vector3& Vector3::operator/=(const float& v) {
	x /= v;
	y /= v;
	z /= v;
	return *this;
}
// Vector3・Vector3（/）
Vector3 Vector3::operator/(const Vector3& v) { return Vector3(x / v.x, y / v.y, z / v.z); }
// Vector3・Vector3（/=）
Vector3& Vector3::operator/=(const Vector3& v) {
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}
// const Vector3・float（/）
Vector3 Vector3::operator/(const float& v) const { return Vector3(x / v, y / v, z / v); }
// const Vector3・Vector3（/）
Vector3 Vector3::operator/(const Vector3& v) const { return Vector3(x / v.x, y / v.y, z / v.z); }

///-------------------------------------------/// 
/// 符号反転
///-------------------------------------------///	
// Vector3 | const
Vector3 Vector3::operator-() const {return Vector3{ -x, -y, -z };}

///=====================================================///
/// 内積の計算
///=====================================================///
float Dot(const Vector3& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

///=====================================================///
/// 長さの計算
///=====================================================///
float Length(const Vector3& v) { return std::sqrtf(Dot(v)); }
float Length(const Vector3& v1, const Vector3& v2) { return std::sqrtf(Dot(v1, v2)); }

///=====================================================///
/// 正規化
///=====================================================///
Vector3 Normalize(const Vector3& v) {
	float mag = Length(v);
	if (mag != 0.0f) {
		return { v.x / mag, v.y / mag, v.z / mag };
	}
	// ゼロベクトルの場合はそのまま返す
	return v;
}