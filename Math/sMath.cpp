#include "sMath.h"
#include <cassert>
#include <numbers>
#include "MatrixMath.h"

///=====================================================///
/// π（πを使用する際に用いる関数）
///=====================================================///
float Math::Pi() { return std::numbers::pi_v<float>; }

///=====================================================///
/// クロス積の計算
///=====================================================///
Vector3 Math::Cross(const Vector3& v1, const Vector3& v2) {

    Vector3 result{ v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };

    return result;
}

///-------------------------------------------/// 
/// Quaternion
///-------------------------------------------///
// QuaternionのNormを返す
float Math::Norm(const Quaternion& quaternion) {
    return sqrtf(quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z +
        quaternion.w * quaternion.w);
}
// QuaternionのNormの2乗を返す
float Math::NormSquared(const Quaternion& quaternion) {
    return quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z +
        quaternion.w * quaternion.w;
}
// 単位Quaternionを返す
Quaternion Math::IdentityQuaternion() {
    return { 0.0f, 0.0f, 0.0f, 1.0f };
}
// 共役Quaternionを返す
Quaternion Math::Conjugate(const Quaternion& quaternion) {
    return {
        -quaternion.x,
        -quaternion.y,
        -quaternion.z,
        quaternion.w
    };
}
// 逆Quaternionを返す
Quaternion Math::Inverse(const Quaternion& quaternion) {
    float normSquared = quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z +
        quaternion.w * quaternion.w;
    if (normSquared == 0.0f) {
        // Avoid division by zero
        return IdentityQuaternion();
    }
    Quaternion conjugate = Conjugate(quaternion);
    return {
        conjugate.x / normSquared,
        conjugate.y / normSquared,
        conjugate.z / normSquared,
        conjugate.w / normSquared
    };
}
// 任意軸回転Quaternionを返す
Quaternion Math::MakeRotateAxisAngle(const Vector3& axis, float angle) {
    Quaternion result;
    float halfAngle = angle / 2.0f;
    float sinHalfAngle = sinf(halfAngle);
    result.x = axis.x * sinHalfAngle;
    result.y = axis.y * sinHalfAngle;
    result.z = axis.z * sinHalfAngle;
    result.w = cosf(halfAngle);
    return result;
}
// Quaternionの回転結果をベクトルで返す
Vector3 Math::RotateVector(const Vector3& vector, const Quaternion& quaternion) {
    Vector3 result;
    Quaternion q = quaternion;
    Quaternion v = { vector.x,vector.y,vector.z,0.0f };
    Quaternion qConjugate = Conjugate(q);
    Quaternion v1 = Multiply(q, v);
    Quaternion v2 = Multiply(v1, qConjugate);
    result.x = v2.x;
    result.y = v2.y;
    result.z = v2.z;
    return result;
}
// QuaternionからX軸の角度を取る関数
float Math::GetXAngle(const Quaternion& quaternion) {
    // 単位ベクトル (0,0,1) をクォータニオンで回転させる
    Vector3 forward = RotateVector(Vector3(0.0f, 0.0f, 1.0f), quaternion);
    // forward.y がピッチ角度（上向きか下向きか）を示す
    return std::asin(forward.y); // ラジアン角
}
// QuaternionからY軸の角度を取る関数
float Math::GetYAngle(const Quaternion& quaternion) {
    // 単位ベクトル (0,0,1) をクォータニオンで回転させる
    Vector3 forward = RotateVector(Vector3(0.0f, 0.0f, 1.0f), quaternion);
    // atan2(forward.x, forward.z) で Yaw（水平回転） を取得。
    return std::atan2(forward.x, forward.z); // Yaw 角度（ラジアン）
}
// QuaternionをEulerAngles(Vector3)に変換する関数
Vector3 Math::QuaternionToEuler(const Quaternion& quaternion) {
    Vector3 euler;

    // Yaw (ヨー: Y軸回転)
    euler.y = std::atan2(2.0f * (quaternion.w * quaternion.y + quaternion.x * quaternion.z), 1.0f - 2.0f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z));

    // Pitch (ピッチ: X軸回転)
    float sinp = 2.0f * (quaternion.w * quaternion.x - quaternion.y * quaternion.z);
    if (std::abs(sinp) >= 1.0f)
        euler.x = std::copysign(Pi() / 2.0f, sinp); // ±90度にクランプ
    else
        euler.x = std::asin(sinp);

    // Roll (ロール: Z軸回転)
    euler.z = std::atan2(2.0f * (quaternion.w * quaternion.z + quaternion.x * quaternion.y), 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.z * quaternion.z));

    return euler;
}
// Vector3からQuaternionに変換する関数
Quaternion Math::QuaternionFromVector(const Vector3& vector) {
    float cx = std::cos(vector.x * 0.5f);
    float sx = std::sin(vector.x * 0.5f);
    float cy = std::cos(vector.y * 0.5f);
    float sy = std::sin(vector.y * 0.5f);
    float cz = std::cos(vector.z * 0.5f);
    float sz = std::sin(vector.z * 0.5f);

    Quaternion q;
    q.w = cx * cy * cz + sx * sy * sz;
    q.x = sx * cy * cz - cx * sy * sz;
    q.y = cx * sy * cz + sx * cy * sz;
    q.z = cx * cy * sz - sx * sy * cz;
    return q;
}
// ある方向（forward）を向くクォータニオン（回転）を作る
Quaternion Math::LookRotation(Vector3 forward, Vector3 up) {
    // Z軸（前方ベクトル）
    Vector3 z = Normalize(forward);
    // X軸（右方向）: forward と up の外積
    Vector3 x = Normalize(Cross(up, z));
    // Y軸（上方向）: Z と X の外積（修正された up）
    Vector3 y = Cross(z, x);

    // 回転行列を作成
    Matrix4x4 rotationMatrix = {
        x.x, y.x, z.x, 0.0f,
        x.y, y.y, z.y, 0.0f,
        x.z, y.z, z.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    // 行列をクォータニオンに変換して返す
    return MatrixToQuaternion(rotationMatrix);
}

///-------------------------------------------/// 
/// 軸の回転を行うQuaternionを返す関数
///-------------------------------------------///
Quaternion Math::RotateX(float angle) {
    float half = angle * 0.5f;
    return Quaternion(
        std::sin(half) * 1.0f, // x
        std::sin(half) * 0.0f, // y
        std::sin(half) * 0.0f, // z
        std::cos(half)         // w
    );
}

Quaternion Math::RotateY(float angle) {
    float half = angle * 0.5f;
    return Quaternion(
        std::sin(half) * 0.0f, // x
        std::sin(half) * 1.0f, // y
        std::sin(half) * 0.0f, // z
        std::cos(half)         // w
    );
}

Quaternion Math::RotateZ(float angle) {
    float half = angle * 0.5f;
    return Quaternion(
        std::sin(half) * 0.0f, // x
        std::sin(half) * 0.0f, // y
        std::sin(half) * 1.0f, // z
        std::cos(half)         // w
    );
}

///-------------------------------------------/// 
/// 
///-------------------------------------------///
// 
float Math::NormalizeAngle(float angle) {
    // 角度を [-π, π] に正規化
    while (angle > std::numbers::pi_v<float>) angle -= 2.0f * std::numbers::pi_v<float>;
    while (angle < -std::numbers::pi_v<float>) angle += 2.0f * std::numbers::pi_v<float>;
    return angle;
}