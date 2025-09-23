#pragma once
/// ===Include=== ///
#include "Engine/Camera/NormalCaemra.h"
#include "Engine/Camera/FollowCamera.h"

/// ===CameraType=== ///
enum class CameraType {
	Normal,   // 通常カメラ
	Follow,   // 追従カメラ
};

///-------------------------------------------/// 
/// Gameで使用するカメラ
///-------------------------------------------///
class GameCamera {
public:

	GameCamera() = default;
	~GameCamera();

	// 初期化
	void Init(CameraType type);
	// 更新
	void Update();
	// カメラ情報
	virtual void ImGuiUpdate();
	// デバッグ用の更新
	void DebugUpdate();
	// カメラシェイク開始
	void StartShake(float intensity, float duration, float frequency = 10.0f);
	// カメラシェイク停止
	void StopShake();

public: /// ===Getter=== ///
	// WorldMatrix
	const Matrix4x4& GetWorldMatrix()const;
	// ViewMatrix
	const Matrix4x4& GetViewMatrix()const;
	// ProjectionMatrix
	const Matrix4x4& GetProjectionMatrix()const;
	// ViewProjectionMatrix
	const Matrix4x4& GetViewProjectionMatrix()const;
	// Translate
	const Vector3& GetTranslate()const;
	// Rotate
	const Quaternion& GetRotate()const;

public: /// ===Setter=== ///
	// Translate
	void SetTranslate(const Vector3& translate);
	// Rotate
	void SetRotate(const Quaternion& rotate);
	// ForY
	void SetForY(const float& forY);
	// Aspect
	void SetAspectRatio(const float& aspect);
	// NearClip
	void SetNearClip(const float& nearClip);
	// FarClip
	void SetFarClip(const float& farClip);
	// FollowTypeの設定
	void SetFollowCamera(FollowCameraType type);
	// 追従対象の座標を設定
	void SetTarget(Vector3* position, Quaternion* rotation);
	// 追従オフセット
	void SetOffset(const Vector3& offset);
	// 回転型追従カメラのオフセット
	void SetOrbitingOffset(const Vector3& offset);
	// 追従速度を設定
	void SetFollowSpeed(float speed);
	// 回転補間速度を設定
	void SetLerpSpeed(float speed);
	// スティック入力
	void SetStick(const Vector2& stickValue);

private:

	/// ===クラス=== ///
	std::shared_ptr<NormalCaemra> normal_; // 通常カメラ
	std::shared_ptr<FollowCamera> follow_; // 追従カメラ

	/// ===カメラタイプ=== ///
	CameraType type_ = CameraType::Normal;
};

