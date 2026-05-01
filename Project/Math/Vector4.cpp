#include "Vector4.h"
#include "Math/sMath.h"

///-------------------------------------------/// 
/// オペレータ
///-------------------------------------------///
 /// === 加算 === ///
Vector4 Vector4::operator+(const float& v) { return Vector4{ x + v, y + v, z + v, w + v }; }
Vector4& Vector4::operator+=(const float& v) {
    x += v; y += v; z += v; w += v;
    return *this;
}
Vector4 Vector4::operator+(const Vector4& q) { return Vector4{ x + q.x, y + q.y, z + q.z, w + q.w }; }
Vector4& Vector4::operator+=(const Vector4& q) {
    x += q.x; y += q.y; z += q.z; w += q.w;
    return *this;
}
Vector4 Vector4::operator+(const float& v) const { return Vector4{ x + v, y + v, z + v, w + v }; }
Vector4 Vector4::operator+(const Vector4& q) const { return Vector4{ x + q.x, y + q.y, z + q.z, w + q.w }; }

/// === 減算 === ///
Vector4 Vector4::operator-(const float& v) { return Vector4{ x - v, y - v, z - v, w - v }; }
Vector4& Vector4::operator-=(const float& v) {
    x -= v; y -= v; z -= v; w -= v;
    return *this;
}
Vector4 Vector4::operator-(const Vector4& q) { return Vector4{ x - q.x, y - q.y, z - q.z, w - q.w }; }
Vector4& Vector4::operator-=(const Vector4& q) {
    x -= q.x; y -= q.y; z -= q.z; w -= q.w;
    return *this;
}
Vector4 Vector4::operator-(const float& v) const { return Vector4{ x - v, y - v, z - v, w - v }; }
Vector4 Vector4::operator-(const Vector4& q) const { return Vector4{ x - q.x, y - q.y, z - q.z, w - q.w }; }

/// === 乗算 === ///
// スカラー倍
Vector4 Vector4::operator*(const float& v) { return Vector4{ x * v, y * v, z * v, w * v }; }
Vector4& Vector4::operator*=(const float& v) {
    x *= v; y *= v; z *= v; w *= v;
    return *this;
}
Vector4 Vector4::operator*(const float& v) const { return Vector4{ x * v, y * v, z * v, w * v }; }
Vector4 Vector4::operator*(const Vector4& q) {
    return Vector4{
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w,
        w * q.w - x * q.x - y * q.y - z * q.z
    };
}
Vector4& Vector4::operator*=(const Vector4& q) {
    *this = *this * q;
    return *this;
}
Vector4 Vector4::operator*(const Vector4& q) const {
    return Vector4{
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w,
        w * q.w - x * q.x - y * q.y - z * q.z
    };
}

/// === 除算 === ///
Vector4 Vector4::operator/(const float& v) { return Vector4{ x / v, y / v, z / v, w / v }; }
Vector4& Vector4::operator/=(const float& v) {
    x /= v; 
    y /= v; 
    z /= v; 
    w /= v;
    return *this;
}
Vector4 Vector4::operator/(const Vector4& q) { return Vector4(x / q.x, y / q.y, z / q.z, w / q.w); }
Vector4& Vector4::operator/=(const Vector4& q) {
	x /= q.x;
	y /= q.y;
	z /= q.z;
	w /= q.w;
	return *this;
}
Vector4 Vector4::operator/(const float& v) const { return Vector4{ x / v, y / v, z / v, w / v }; }
Vector4 Vector4::operator/(const Vector4& q) const { return Vector4(x / q.x, y / q.y, z / q.z, w / q.w); }

///=====================================================///
/// 内積の計算
///=====================================================///
float Dot(const Vector4& q) { return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w; }
float Dot(const Vector4& q1, const Vector4& q2) { return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w; }

///=====================================================///
/// 長さの計算
///=====================================================///
float Length(const Vector4& q) { return std::sqrtf(Dot(q)); }
float Length(const Vector4& q1, const Vector4& q2) { return std::sqrtf(Dot(q1, q2)); }

///-------------------------------------------/// 
/// 正規化
///-------------------------------------------///
Vector4 Normalize(const Vector4& Vector4) {
    float mag = Length(Vector4);
    if (mag != 0.0f) {
		return { Vector4.x / mag, Vector4.y / mag, Vector4.z / mag, Vector4.w / mag };
    };
	// ゼロベクトルの場合はそのまま返す
	return Vector4;
}