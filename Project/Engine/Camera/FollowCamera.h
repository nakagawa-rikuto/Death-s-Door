#pragma once
/// ===Include=== ///
#include "NormalCaemra.h"

/// === カメラの種類を表す列挙型 === ///
enum class FollowCameraType {
	FixedOffset,        // 固定オフセット型（一定のオフセット距離で追従）
	Interpolated,       // スムージング追従型（補間で滑らかに追従）
	Orbiting,           // 回転可能型（対象の周りを回るカメラ）
	CollisionAvoidance, // 衝突回避型（障害物を避ける）
	TopDown,           // 上からの視点
};

///=====================================================/// 
/// 追従カメラクラス
///=====================================================///
class FollowCamera : public NormalCaemra {
public:
	FollowCamera() = default;
	~FollowCamera() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// FollowTypeの設定
	/// </summary>
	void SetFollowCamera(FollowCameraType type);

public:/// ===Setter=== ///
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

private:/// ===変数=== ///
	/// ===追従=== ///
	Vector3* targetPos_ = nullptr;  // 追従対象の座標ポインタ
	Quaternion* targetRot_ = nullptr;  // 追従対象の回転ポインタ
	Vector3 offset_ = { 0.0f, 0.0f, -20.0f }; // カメラの初期オフセット
	Vector3 OrbitingOffset_ = { 0.0f, 0.5f, -20.0f }; // 回転カメラ用オフセット
	float followSpeed_ = 0.1f;      // 追従速度
	float rotationLerpSpeed_ = 0.1f; // 回転補間速度

	Vector2 stickValue_ = { 0.0f, 0.0f }; // スティック入力値

	FollowCameraType cameraType_ = FollowCameraType::FixedOffset; // デフォルトカメラタイプ

private:
	// カメラの種類に応じた更新処理
	void UpdateFollowCamera();
	// 固定オフセット型カメラの処理 
	void FollowFixedOffset();
	// スムージング追従型カメラの処理 
	void FollowInterpolated();
	// 回転可能型カメラの処理 
	void FollowOrbiting();
	// 衝突回避型カメラの処理 
	void FollowCollisionAvoidance();
	// 上からの見下ろし追従カメラの処理
	void FollowTopDown();
};