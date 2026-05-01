#include "sMath.h"
#include <cassert>
#include <numbers>
#include "MatrixMath.h"

///=====================================================///
/// π（πを使用する際に用いる関数）
///=====================================================///
float Math::Pi() { return std::numbers::pi_v<float>; }

///-------------------------------------------/// 
/// 角度の正規化
///-------------------------------------------/// 
float Math::NormalizeAngle(float angle) {
    // 角度を [-π, π] に正規化
    while (angle > std::numbers::pi_v<float>) angle -= 2.0f * std::numbers::pi_v<float>;
    while (angle < -std::numbers::pi_v<float>) angle += 2.0f * std::numbers::pi_v<float>;
    return angle;
}

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
    Quaternion result = {};
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
    Vector3 result = {};
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
    Vector3 euler = {};

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
    // 度数をラジアンに変換する (degree * pi / 180)
    float radX = vector.x * (Pi() / 180.0f);
    float radY = vector.y * (Pi() / 180.0f);
    float radZ = vector.z * (Pi() / 180.0f);

    float cx = std::cos(radX * 0.5f);
    float sx = std::sin(radX * 0.5f);
    float cy = std::cos(radY * 0.5f);
    float sy = std::sin(radY * 0.5f);
    float cz = std::cos(radZ * 0.5f);
    float sz = std::sin(radZ * 0.5f);

    Quaternion q = {};

    // Q = Qz * Qy * Qx (各軸の回転をこの順で適用する場合)
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
// X軸回転Quaternionを返す関数
Quaternion Math::RotateX(float angle) {
    float half = angle * 0.5f;
    return Quaternion(
        std::sin(half) * 1.0f, // x
        std::sin(half) * 0.0f, // y
        std::sin(half) * 0.0f, // z
        std::cos(half)         // w
    );
}
// Y軸回転Quaternionを返す関数
Quaternion Math::RotateY(float angle) {
    float half = angle * 0.5f;
    return Quaternion(
        std::sin(half) * 0.0f, // x
        std::sin(half) * 1.0f, // y
        std::sin(half) * 0.0f, // z
        std::cos(half)         // w
    );
}
// Z軸回転Quaternionを返す関数
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
/// ピッチ回転を適用したQuaternionを返す関数
///-------------------------------------------///
Quaternion Math::ApplyPitchToCurrentRotation(const Quaternion& currentYRotation, float pitchAngleDegrees) {
    // X軸周りの回転クォータニオンを作成
    float halfAngle = pitchAngleDegrees * 0.5f * (Pi() / 180.0f); // ラジアンに変換
    Quaternion pitchRotation = {};
    pitchRotation.x = sinf(halfAngle);
    pitchRotation.y = 0.0f;
    pitchRotation.z = 0.0f;
    pitchRotation.w = cosf(halfAngle);

    // 現在のY軸回転に、X軸回転を合成
    Quaternion result = {};

    float w1 = currentYRotation.w, x1 = currentYRotation.x;
    float y1 = currentYRotation.y, z1 = currentYRotation.z;
    float w2 = pitchRotation.w, x2 = pitchRotation.x;
    float y2 = pitchRotation.y, z2 = pitchRotation.z;

    result.w = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2;
    result.x = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2;
    result.y = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2;
    result.z = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2;

    return result;
}

///-------------------------------------------/// 
/// Lerp関数
///-------------------------------------------///
// float
float Math::Lerp(float start, float end, float t) {
    return start * (1.0f - t) + end * t;
}
// Vector3
Vector3 Math::Lerp(const Vector3& start, const Vector3& end, float t) {
    Vector3 result = {};

    result.x = (1.0f - t) * start.x + t * end.x;
    result.y = (1.0f - t) * start.y + t * end.y;
    result.z = (1.0f - t) * start.z + t * end.z;

    return result;
}
// Quaternion
Quaternion Math::Lerp(const Quaternion& start, const Quaternion& end, float t) {
    Quaternion result = {};

    result.x = (1.0f - t) * start.x + t * end.x;
    result.y = (1.0f - t) * start.y + t * end.y;
    result.z = (1.0f - t) * start.z + t * end.z;
    result.w = (1.0f - t) * start.w + t * end.w;

    return result;
}
// Vector4
Vector4 Math::Lerp(const Vector4& start, const Vector4& end, float t) {
    Vector4 result = {};

    result.x = (1.0f - t) * start.x + t * end.x;
    result.y = (1.0f - t) * start.y + t * end.y;
    result.z = (1.0f - t) * start.z + t * end.z;
    result.w = (1.0f - t) * start.w + t * end.w;

    return result;
}

///-------------------------------------------/// 
/// SLerp関数
///-------------------------------------------///
// Vector3
Vector3 Math::SLerp(const Vector3& start, const Vector3& end, float t) {
    // なす角の計算
    float angle = std::cosf(Dot(start, end));

    // 線形補間を計算する
    float scaleStart = std::sinf((1.0f - t) * angle) / std::sinf(angle);

    float scaleEnd = std::sinf(t * angle) / std::sinf(angle);

    Vector3 result = {};

    result.x = scaleStart * start.x + scaleEnd * end.x;
    result.y = scaleStart * start.y + scaleEnd * end.y;
    result.z = scaleStart * start.z + scaleEnd * end.z;

    return result;
}
// Quaternion
Quaternion Math::SLerp(const Quaternion& start, const Quaternion& end, float t) {
    Quaternion q2Modified = end;
    float dot = Dot(start, end);

    // 逆方向補間を防ぐために符号を反転
    if (dot < 0.0f) {
        q2Modified = Quaternion(-q2Modified.x, -q2Modified.y, -q2Modified.z, -q2Modified.w);
        dot = -dot;
    }

    // クォータニオン補間
    if (dot > 0.9995f) {
        // 角度が小さい場合は Lerp で近似
        return Normalize(start + (q2Modified - start) * t);
    }

    float theta_0 = acosf(dot); // 初期角度
    float theta = theta_0 * t;  // 補間後の角度

    Quaternion q3 = Normalize(q2Modified - start * dot); // 直交成分
    return start * cosf(theta) + q3 * sinf(theta);
}
// Vector4
Vector4 Math::SLerp(const Vector4& start, const Vector4& end, float t) {
    Vector4 q2Modified = end;
    float dot = Dot(start, end);

    // 逆方向補間を防ぐために符号を反転
    if (dot < 0.0f) {
        q2Modified = Vector4(-q2Modified.x, -q2Modified.y, -q2Modified.z, -q2Modified.w);
        dot = -dot;
    }

    // Vector4補間
    if (dot > 0.9995f) {
        // 角度が小さい場合は Lerp で近似
        return Normalize(start + (q2Modified - start) * t);
    }

    float theta_0 = acosf(dot); // 初期角度
    float theta = theta_0 * t;  // 補間後の角度

    Vector4 q3 = Normalize(q2Modified - start * dot); // 直交成分
    return start * cosf(theta) + q3 * sinf(theta);
}
