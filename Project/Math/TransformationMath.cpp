#include "TransformationMath.h"
// C++
#include <algorithm>
// CameraCommon
#include "Engine/Camera/Base/CameraCommon.h"
// Service
#include "Service/GraphicsResourceGetter.h"
#include "Service/Camera.h"
// Math
#include "sMath.h"
#include "MatrixMath.h"

///-------------------------------------------/// 
/// ワールド座標をスクリーン座標に変換
///-------------------------------------------///
Vector2 Math::WorldToScreen(const Vector3& worldPos) {
    // カメラの取得
    MiiEngine::CameraCommon* camera = Service::Camera::GetActiveCamera();
	// ビュー射影行列の取得
	Matrix4x4 viewProj = camera->GetViewProjectionMatrix();
    // スクリーンの取得
	float width = static_cast<float>(Service::GraphicsResourceGetter::GetWindowWidth());
	float height = static_cast<float>(Service::GraphicsResourceGetter::GetWindowHeight());

	// ワールド座標をスクリーン座標に変換
	Vector3 screenPos3D = Math::TransformCoordinates(worldPos, viewProj);
    Vector2 screenPos = {
        .x = (screenPos3D.x + 1.0f) * 0.5f * width,
        .y = (1.0f - screenPos3D.y) * 0.5f * height
    };

    return screenPos;
}

///-------------------------------------------/// 
/// スクリーン座標をワールド座標に変換
///-------------------------------------------///
Vector3 Math::ScreenToWorld(const Vector2& screenPos) {
    // カメラの取得
    MiiEngine::CameraCommon* camera = Service::Camera::GetActiveCamera();
    // ビュー射影行列の取得
    Matrix4x4 viewProj = camera->GetViewProjectionMatrix();
    // ビュー射影行列の逆行列を取得
    Matrix4x4 invViewProj = Math::Inverse4x4(viewProj);

    // スクリーンの取得
    float width = static_cast<float>(Service::GraphicsResourceGetter::GetWindowWidth());
    float height = static_cast<float>(Service::GraphicsResourceGetter::GetWindowHeight());

    // スクリーン座標をNDC座標に変換
    Vector3 ndcPos = {
        .x = (screenPos.x / width) * 2.0f - 1.0f,
        .y = 1.0f - (screenPos.y / height) * 2.0f,
        .z = 0.0f
    };

    // NDC座標をワールド座標に変換
    Vector3 worldPos = Math::TransformCoordinates(ndcPos, invViewProj);

    return worldPos;
}

///-------------------------------------------/// 
/// QuaternionをEuler角に変換
///-------------------------------------------///
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

///-------------------------------------------/// 
/// Euler角をQuaternionに変換
///-------------------------------------------///
Quaternion Math::EulerToQuaternion(const Vector3& euler) {
    // 度数をラジアンに変換する (degree * pi / 180)
    float radX = euler.x * (Pi() / 180.0f);
    float radY = euler.y * (Pi() / 180.0f);
    float radZ = euler.z * (Pi() / 180.0f);

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