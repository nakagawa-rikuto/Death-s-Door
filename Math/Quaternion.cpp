#include "Quaternion.h"

#include "Math/sMath.h"

///-------------------------------------------/// 
/// オペレータ
///-------------------------------------------///
 /// === 加算 === ///
Quaternion Quaternion::operator+(const float& v) { return Quaternion{ x + v, y + v, z + v, w + v }; }
Quaternion& Quaternion::operator+=(const float& v) {
    x += v; y += v; z += v; w += v;
    return *this;
}
Quaternion Quaternion::operator+(const Quaternion& q) { return Quaternion{ x + q.x, y + q.y, z + q.z, w + q.w }; }
Quaternion& Quaternion::operator+=(const Quaternion& q) {
    x += q.x; y += q.y; z += q.z; w += q.w;
    return *this;
}
Quaternion Quaternion::operator+(const float& v) const { return Quaternion{ x + v, y + v, z + v, w + v }; }
Quaternion Quaternion::operator+(const Quaternion& q) const { return Quaternion{ x + q.x, y + q.y, z + q.z, w + q.w }; }

/// === 減算 === ///
Quaternion Quaternion::operator-(const float& v) { return Quaternion{ x - v, y - v, z - v, w - v }; }
Quaternion& Quaternion::operator-=(const float& v) {
    x -= v; y -= v; z -= v; w -= v;
    return *this;
}
Quaternion Quaternion::operator-(const Quaternion& q) { return Quaternion{ x - q.x, y - q.y, z - q.z, w - q.w }; }
Quaternion& Quaternion::operator-=(const Quaternion& q) {
    x -= q.x; y -= q.y; z -= q.z; w -= q.w;
    return *this;
}
Quaternion Quaternion::operator-(const float& v) const { return Quaternion{ x - v, y - v, z - v, w - v }; }
Quaternion Quaternion::operator-(const Quaternion& q) const { return Quaternion{ x - q.x, y - q.y, z - q.z, w - q.w }; }

/// === 乗算 === ///
// スカラー倍
Quaternion Quaternion::operator*(const float& v) { return Quaternion{ x * v, y * v, z * v, w * v }; }
Quaternion& Quaternion::operator*=(const float& v) {
    x *= v; y *= v; z *= v; w *= v;
    return *this;
}
Quaternion Quaternion::operator*(const float& v) const { return Quaternion{ x * v, y * v, z * v, w * v }; }

// クォータニオン乗算
Quaternion Quaternion::operator*(const Quaternion& q) {
    return Quaternion{
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w,
        w * q.w - x * q.x - y * q.y - z * q.z
    };
}
Quaternion& Quaternion::operator*=(const Quaternion& q) {
    *this = *this * q;
    return *this;
}
Quaternion Quaternion::operator*(const Quaternion& q) const {
    return Quaternion{
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w,
        w * q.w - x * q.x - y * q.y - z * q.z
    };
}

/// === 除算 === ///
Quaternion Quaternion::operator/(const float& v) { return Quaternion{ x / v, y / v, z / v, w / v }; }
Quaternion& Quaternion::operator/=(const float& v) {
    x /= v; y /= v; z /= v; w /= v;
    return *this;
}
Quaternion Quaternion::operator/(const float& v) const { return Quaternion{ x / v, y / v, z / v, w / v }; }

// クォータニオンの除算（逆クォータニオンをかける）
Quaternion Quaternion::operator/(const Quaternion& q) {
    return (*this) * Math::Inverse(q);
}
Quaternion& Quaternion::operator/=(const Quaternion& q) {
    *this = *this / q;
    return *this;
}
Quaternion Quaternion::operator/(const Quaternion& q) const {
    return (*this) * Math::Inverse(q);
}

///=====================================================///
/// 内積の計算
///=====================================================///
float Dot(const Quaternion& q) { return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w; }
float Dot(const Quaternion& q1, const Quaternion& q2) { return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w; }

///=====================================================///
/// 長さの計算
///=====================================================///
float Length(const Quaternion& q) { return std::sqrtf(Dot(q)); }
float Length(const Quaternion& q1, const Quaternion& q2) { return std::sqrtf(Dot(q1, q2)); }

///-------------------------------------------/// 
/// Quaternionの積
///-------------------------------------------///
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
    return {
         lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
         lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
         lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
         lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
    };
}

///-------------------------------------------/// 
/// 正規化
///-------------------------------------------///
Quaternion Normalize(const Quaternion& quaternion) {
    // ノルムの二乗を取得
    float normSquared = Math::NormSquared(quaternion);
    if (normSquared < 1e-6f) { // 0ではなく非常に小さい値でチェック
        return Math::IdentityQuaternion();
    }

    // ノルムを計算
    float norm = sqrtf(normSquared);
    return {
        quaternion.x / norm,
        quaternion.y / norm,
        quaternion.z / norm,
        quaternion.w / norm
    };
}