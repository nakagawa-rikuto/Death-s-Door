#pragma once
/// ===Include=== ///
#include "Engine/DataInfo/CData.h"

///=====================================================/// 
/// カメラ基底クラス
///=====================================================///
class NormalCaemra {
public:
	NormalCaemra() = default;
	virtual ~NormalCaemra() = default;

	// 初期化
	virtual void Initialize();
	// 更新
	virtual void Update();
	// カメラ情報
	virtual void ImGuiUpdate();

	// デバッグ用の更新
	void DebugUpdate();
	// カメラシェイク開始
	void StartShake(float intensity, float duration, float frequency = 10.0f);
	// カメラシェイク停止
	void StopShake();


public:/// ===Getter=== ///
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

public:/// ===Setter=== ///
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

protected:/// ===変数=== ///
	/// ===ビュー行列データ=== ///
	QuaternionTransform transform_;
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;

	/// ===プロジェクション行列データ=== ///
	Matrix4x4 projectionMatrix_;
	float horizontalView_; // 水平方向視野角
	float aspect_;         // アスペクト比
	float nearClip_;       // ニアクリップ距離
	float farClip_;        // ファークリップ距離

	/// ===合成行列=== ///
	Matrix4x4 viewProjectionMatrix_;

	/// ===シェイク=== ///
	bool isShaking_ = false;        // シェイク中フラグ
	float shakeIntensity_ = 0.0f;   // シェイクの強度
	float shakeDuration_ = 0.0f;    // シェイクの継続時間
	float shakeTimer_ = 0.0f;       // シェイクのタイマー
	float shakeFrequency_ = 10.0f;  // シェイクの周波数
	Vector3 shakeOffset_;           // シェイクによるオフセット
	float shakeTimeOffset_;         // ランダムな時間オフセット

private:
	// 行列の更新処理
	void UpdateMatrices();
	// シェイク
	void UpdateShake();
};