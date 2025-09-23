#include "NormalCaemra.h"
// Math
#include "Math/sMath.h"
#include "Math/EasingMath.h"
#include "Math/MatrixMath.h"
// Service
#include "Engine/System/Service/GraphicsResourceGetter.h"
// C++
#include <random>
#include <ctime>
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI
// Debug
#ifdef _DEBUG
#include "Engine/System/Service/InputService.h"
#endif // _DEBUG

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// WorldMatrix
const Matrix4x4& NormalCaemra::GetWorldMatrix() const { return worldMatrix_; }
// ViewMatrix
const Matrix4x4& NormalCaemra::GetViewMatrix() const { return viewMatrix_; }
// ProjectionMatrix
const Matrix4x4& NormalCaemra::GetProjectionMatrix() const { return projectionMatrix_; }
// ViewProjectionMatrix
const Matrix4x4& NormalCaemra::GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
// Translate
const Vector3& NormalCaemra::GetTranslate() const { return transform_.translate; }
// Rotate
const Quaternion& NormalCaemra::GetRotate() const { return transform_.rotate; }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Translate
void NormalCaemra::SetTranslate(const Vector3& translate) { transform_.translate = translate; }
// Rotate
void NormalCaemra::SetRotate(const Quaternion& rotate) { transform_.rotate = rotate; }
// ForY
void NormalCaemra::SetForY(const float& forY) { horizontalView_ = forY; }
// AspectRatio
void NormalCaemra::SetAspectRatio(const float& aspect) { aspect_ = aspect; }
// NearClip
void NormalCaemra::SetNearClip(const float& nearClip) { nearClip_ = nearClip; }
// FarClip
void NormalCaemra::SetFarClip(const float& farClip) { farClip_ = farClip; }

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void NormalCaemra::Initialize() {
	transform_ = { {1.0f, 1.0f, 1.0f},{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} };
	horizontalView_ = 0.45f;
	aspect_ = static_cast<float>(GraphicsResourceGetter::GetWindowWidth()) / static_cast<float>(GraphicsResourceGetter::GetWindowHeight());
	nearClip_ = 0.1f;
	farClip_ = 100.0f;

	UpdateMatrices();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void NormalCaemra::Update() {
	UpdateMatrices();
}


///-------------------------------------------/// 
/// 情報
///-------------------------------------------///
void NormalCaemra::ImGuiUpdate() {
#ifdef USE_IMGUI
	ImGui::Begin("Caemra");
	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat4("Rotate", &transform_.rotate.x, 0.001f);
	ImGui::DragFloat("Horizontal View", &horizontalView_, 0.01f);
	ImGui::DragFloat("Aspect Ratio", &aspect_, 0.01f);
	ImGui::DragFloat("Near Clip", &nearClip_, 0.01f);
	ImGui::DragFloat("Far Clip", &farClip_, 0.01f);
	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// デバッグ用の更新
///-------------------------------------------///
void NormalCaemra::DebugUpdate() {
#ifdef _DEBUG
	// カメラのローカルX軸（右方向ベクトル）を取得
	Vector3 right = Math::RotateVector({ 1.0f, 0.0f, 0.0f }, transform_.rotate);

	/// ===カメラの移動=== ///
	if (InputService::PushKey(DIK_D)) {
		transform_.translate.x += 0.01f;
	} else if (InputService::PushKey(DIK_A)) {
		transform_.translate.x -= 0.01f;
	}
	if (InputService::PushKey(DIK_W)) {
		transform_.translate.z += 0.01f;
	} else if (InputService::PushKey(DIK_S)) {
		transform_.translate.z -= 0.01f;
	}
	if (InputService::PushKey(DIK_SPACE)) {
		transform_.translate.y += 0.01f;
	} else if (InputService::PushKey(DIK_LSHIFT)) {
		transform_.translate.y -= 0.01f;
	}

	// ピッチ（縦回転）
	if (InputService::PushKey(DIK_UP)) {
		Quaternion delta = Math::MakeRotateAxisAngle(right, -0.01f);
		transform_.rotate = delta * transform_.rotate;
	} else if (InputService::PushKey(DIK_DOWN)) {
		Quaternion delta = Math::MakeRotateAxisAngle(right, +0.01f);
		transform_.rotate = delta * transform_.rotate;
	}

	// ヨー（左右回転）
	if (InputService::PushKey(DIK_LEFT)) {
		Quaternion delta = Math::RotateY(-0.01f);
		transform_.rotate = delta * transform_.rotate;
	} else if (InputService::PushKey(DIK_RIGHT)) {
		Quaternion delta = Math::RotateY(+0.01f);
		transform_.rotate = delta * transform_.rotate;
	}
#endif // _DEBUG
}

///-------------------------------------------/// 
/// シェイク開始処理
///-------------------------------------------///
void NormalCaemra::StartShake(float intensity, float duration, float frequency) {
	isShaking_ = true;
	shakeIntensity_ = intensity;
	shakeDuration_ = duration;
	shakeTimer_ = 0.0f;
	shakeFrequency_ = frequency;

	// ランダムな時間オフセットを生成（各軸で異なる波形にするため）
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.0f, 2.0f * 3.14159f);
	shakeTimeOffset_ = dist(gen);
}

///-------------------------------------------/// 
/// シェイク処理終了
///-------------------------------------------///
void NormalCaemra::StopShake() {
	isShaking_ = false;
	shakeTimer_ = 0.0f;
	shakeOffset_ = { 0.0f, 0.0f, 0.0f };
}

///-------------------------------------------/// 
/// 行列の更新処理
///-------------------------------------------///
void NormalCaemra::UpdateMatrices() {
	/// ===シェイク=== ///
	// シェイクの更新
	UpdateShake();
	// シェイクオフセット
	Vector3 finalPosition = transform_.translate + shakeOffset_;

	// 行列の計算
	worldMatrix_ = Math::MakeAffineQuaternionMatrix(transform_.scale, transform_.rotate, finalPosition);
	viewMatrix_ = Math::Inverse4x4(worldMatrix_);

	// プロジェクション行列の更新
	projectionMatrix_ = Math::MakePerspectiveFovMatrix(horizontalView_, aspect_, nearClip_, farClip_);

	// 合成行列
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

///-------------------------------------------/// 
/// シェイク処理
///-------------------------------------------///
void NormalCaemra::UpdateShake() {
	if (!isShaking_) {
		shakeOffset_ = { 0.0f, 0.0f, 0.0f };
		return;
	}

	// タイマーを進める（デルタタイムは適宜調整）
	const float deltaTime = 1.0f / 60.0f; // 仮に60FPSとして計算
	shakeTimer_ += deltaTime;

	// シェイク終了判定
	if (shakeTimer_ >= shakeDuration_) {
		StopShake();
		return;
	}

	// 減衰カーブ（時間経過とともに弱くなる）
	float decayFactor = 1.0f - (shakeTimer_ / shakeDuration_);
	decayFactor = decayFactor * decayFactor; // 二乗で急激な減衰

	// Sin波を使ったシェイク計算（各軸で異なる位相）
	float time = shakeTimer_ * shakeFrequency_;

	shakeOffset_.x = sin(time + shakeTimeOffset_) * shakeIntensity_ * decayFactor;
	shakeOffset_.y = sin(time + shakeTimeOffset_ + 1.57f) * shakeIntensity_ * decayFactor * 0.7f; // Y軸は少し弱く
	shakeOffset_.z = sin(time + shakeTimeOffset_ + 3.14f) * shakeIntensity_ * decayFactor * 0.5f; // Z軸はさらに弱く

	// ランダム要素を追加してより自然に
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> randomDist(-0.3f, 0.3f);

	shakeOffset_.x += randomDist(gen) * shakeIntensity_ * decayFactor;
	shakeOffset_.y += randomDist(gen) * shakeIntensity_ * decayFactor * 0.7f;
	shakeOffset_.z += randomDist(gen) * shakeIntensity_ * decayFactor * 0.5f;
}
