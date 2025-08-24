#include "Camera.h"
// Math
#include "Math/sMath.h"
#include "Math/EasingMath.h"
#include "Math/MatrixMath.h"
// Service
#include "Engine/System/Service/GraphicsResourceGetter.h"
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI
// Debug
#ifdef _DEBUG
#include "Engine/System/Service/InputService.h"
#endif // _DEBUG


///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
Camera::~Camera() {
	targetPos_ = nullptr;
	targetRot_ = nullptr;
}

///-------------------------------------------/// 
/// FollowCameraの設定
///-------------------------------------------///
void Camera::SetFollowCamera(FollowCameraType type) {
	cameraType_ = type;
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// WorldMatrix
const Matrix4x4& Camera::GetWorldMatrix() const { return worldMatrix_; }
// ViewMatrix
const Matrix4x4& Camera::GetViewMatrix() const { return viewMatrix_; }
// ProjectionMatrix
const Matrix4x4& Camera::GetProjectionMatrix() const { return projectionMatrix_; }
// ViewProjectionMatrix
const Matrix4x4& Camera::GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
// Translate
const Vector3& Camera::GetTranslate() const { return transform_.translate; }
// Rotate
const Quaternion& Camera::GetRotate() const { return transform_.rotate; }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Translate
void Camera::SetTranslate(const Vector3& translate) { addTransform_.translate = translate; }
// Rotate
void Camera::SetRotate(const Quaternion& rotate) { addTransform_.rotate = rotate; }
// ForY
void Camera::SetForY(const float& forY) { horizontalView_ = forY; }
// AspectRatio
void Camera::SetAspectRatio(const float& aspect) { aspect_ = aspect; }
// NearClip
void Camera::SetNearClip(const float& nearClip) { nearClip_ = nearClip; }
// FarClip
void Camera::SetFarClip(const float& farClip) { farClip_ = farClip; }
// 追従対象の座標を設定
void Camera::SetTarget(Vector3* position, Quaternion* rotation) {
	targetPos_ = position;
	targetRot_ = rotation;
}
// 追従のオフセット
void Camera::SetOffset(const Vector3& offset) { offset_ = offset; }
void Camera::SetOrbitingOffset(const Vector3& offset) { OrbitingOffset_ = offset; }
// 追従速度を設定
void Camera::SetFollowSpeed(float speed) { followSpeed_ = speed; }
// 回転補間速度
void Camera::SetLerpSpeed(float speed) { rotationLerpSpeed_ = speed; }
// 回転の量
void Camera::SetStick(const Vector2& stickValue) { stickValue_ = stickValue; }

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Camera::Initialize() {
	transform_ = { {1.0f, 1.0f, 1.0f},{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} };
	addTransform_ = { {1.0f, 1.0f, 1.0f},{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.f} };
	horizontalView_ = 0.45f;
	aspect_ = static_cast<float>(GraphicsResourceGetter::GetWindowWidth()) / static_cast<float>(GraphicsResourceGetter::GetWindowHeight());
	nearClip_ = 0.1f;
	farClip_ = 100.0f;
	worldMatrix_ = Math::MakeAffineQuaternionMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Math::Inverse4x4(worldMatrix_);
	projectionMatrix_ = Math::MakePerspectiveFovMatrix(horizontalView_, aspect_, nearClip_, farClip_);
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}


///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Camera::Update() {

	if (targetPos_ && targetRot_) {
		// 追従処理
		//FollowTarget();
		//PreFollowTarget();
		UpdateFollowCamera();
	} else {
		transform_ = addTransform_;
	}

	// 行列の計算
	worldMatrix_ = Math::MakeAffineQuaternionMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Math::Inverse4x4(worldMatrix_);

	// プロジェクション行列の更新
	projectionMatrix_ = Math::MakePerspectiveFovMatrix(horizontalView_, aspect_, nearClip_, farClip_);

	// 合成行列
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

///-------------------------------------------/// 
/// デバッグ用の更新
///-------------------------------------------///
void Camera::DebugUpdate() {
#ifdef _DEBUG
	// カメラのローカルX軸（右方向ベクトル）を取得
	Vector3 right = Math::RotateVector({ 1.0f, 0.0f, 0.0f }, transform_.rotate);

	/// ===カメラの回転=== ///
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
/// 情報
///-------------------------------------------///
void Camera::UpdateImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("Camera");
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
/// 追従処理
///-------------------------------------------///
// 
void Camera::UpdateFollowCamera() {
	switch (cameraType_) {
	case FollowCameraType::FixedOffset:
		FollowFixedOffset();
		break;
	case FollowCameraType::Interpolated:
		FollowInterpolated();
		break;
	case FollowCameraType::Orbiting:
		FollowOrbiting();
		break;
	case FollowCameraType::CollisionAvoidance:
		FollowCollisionAvoidance();
		break;
	case FollowCameraType::TopDown:
		FollowTopDown();
		break;
	}
}
// 回転軸がY座標だけの追従処理
void Camera::FollowFixedOffset() {
	// 目標の回転（Quaternion）の Yaw 成分のみを取得
	float targetYaw = Math::GetYAngle(*targetRot_);
	Quaternion targetYawRotation = Math::MakeRotateAxisAngle(Vector3(0, 1, 0), targetYaw);

	// オフセットをターゲットの Y 軸回転に基づいて回転
	Vector3 rotatedOffset = Math::RotateVector(offset_, targetYawRotation);

	// 目標位置を算出
	Vector3 targetCameraPos = *targetPos_ + rotatedOffset;

	// カメラの位置を補間（Lerp）で滑らかに移動
	transform_.translate = Math::Lerp(transform_.translate, targetCameraPos, followSpeed_);

	// カメラの回転を Yaw のみに制限して補間
	transform_.rotate = Math::SLerp(transform_.rotate, targetYawRotation, rotationLerpSpeed_);
}
// 回転軸がXとY座標の追従処理
void Camera::FollowInterpolated() {
	// 目標の回転（Quaternion）を取得
	Quaternion targetRotation = *targetRot_;

	// オフセットをターゲットの回転に基づいて回転
	Vector3 rotatedOffset = Math::RotateVector(offset_, targetRotation);

	// 目標位置を算出
	Vector3 targetCameraPos = *targetPos_ + rotatedOffset;

	// カメラの位置を補間（Lerp）で滑らかに移動
	transform_.translate = Math::Lerp(transform_.translate, targetCameraPos, followSpeed_);

	// カメラの回転をスムーズに補間（Slerp を使用）
	//transform_.rotate = Math::SLerp(transform_.rotate, targetRotation, rotationLerpSpeed_);
}
// 自分の周りをまわるカメラの追従処理
void Camera::FollowOrbiting() {
	// クォータニオンで回転を管理
	Quaternion rotationDelta = Math::IdentityQuaternion();

	// 右スティックのX・Y軸の値を取得 (-32768 ～ 32767)
	float rightStickX = stickValue_.x; // Yaw（左右回転）
	float rightStickY = stickValue_.y; // Pitch（上下回転）

	// デッドゾーン処理（スティックがわずかに傾いたときの無効化）
	const float DEADZONE = 0.2f;
	if (fabs(rightStickX) < DEADZONE) rightStickX = 0.0f;
	if (fabs(rightStickY) < DEADZONE) rightStickY = 0.0f;

	// スティックの入力を回転量に変換
	float deltaYaw = rightStickX * 0.05f;  // 感度調整
	float deltaPitch = rightStickY * 0.05f;

	// クォータニオンを用いた回転計算
	Quaternion yawRotation = Math::MakeRotateAxisAngle(
		Vector3(0, 1, 0), deltaYaw);
	Quaternion pitchRotation = Math::MakeRotateAxisAngle(
		Math::RotateVector(Vector3(1, 0, 0), yawRotation * transform_.rotate), deltaPitch);

	// 回転の補間
	rotationDelta = pitchRotation * yawRotation;


	// 累積回転を更新
	transform_.rotate = rotationDelta * transform_.rotate;

	offset_ = OrbitingOffset_;

	// 回転を適用
	offset_ = Math::RotateVector(offset_, transform_.rotate);
	transform_.translate = offset_ + *targetPos_;

	transform_.rotate = Normalize(transform_.rotate); // クォータニオンを正規化して数値誤差を防ぐ
}
// 障害物を避ける追従処理
void Camera::FollowCollisionAvoidance() {
	// プレイヤーの回転を基にY軸回転行列を作成
	Matrix4x4 rotationMatrix = Math::MakeRotateYMatrix(targetRot_->y);

	// オフセットを回転行列で変換し、適切な位置に調整
	Vector3 rotatedOffset = Math::TransformVector(offset_, rotationMatrix);

	// 目標位置を算出
	Vector3 targetCameraPos = *targetPos_ + rotatedOffset;

	// 障害物の有無をチェック（レイキャストを使用予定）
	bool hitWall = false; // ここにレイキャストを実装する（仮）

	// 壁がある場合はターゲットに寄る
	if (hitWall) {
		transform_.translate = Math::Lerp(transform_.translate, *targetPos_, followSpeed_);
	} else {
		transform_.translate = Math::Lerp(transform_.translate, targetCameraPos, followSpeed_);
	}

	// カメラの向きをプレイヤーの回転に合わせる
	transform_.rotate = *targetRot_;
}

///-------------------------------------------/// 
/// 上からの見下ろしカメラの追従処理
///-------------------------------------------///
void Camera::FollowTopDown() {

	const float deadzone = 0.1f;
	const float radius = 15.0f; // 回転半径（お好みで）

	Vector3 desiredPosition = *targetPos_ + offset_; // デフォルト位置（追従対象の真上）

	// スティック入力がある場合はカメラを円運動させる
	if (fabsf(stickValue_.x) > deadzone || fabsf(stickValue_.y) > deadzone) {
		// 入力ベクトルの正規化（方向ベクトル）
		Vector2 norm = Normalize(stickValue_);

		// 円上の位置を計算
		Vector3 offsetCircle = {
			norm.x * radius,
			offset_.y,           // Y（高さ）は固定
			norm.y * radius
		};

		Vector3 translate = offset_ + *targetPos_;
		translate.y = 1.0f;

		desiredPosition = translate + offsetCircle;
	}

	// カメラの位置を補間で滑らかに移動
	transform_.translate = Math::Lerp(transform_.translate, desiredPosition, followSpeed_);

	// 対象を見下ろすように回転
	/*Vector3 forward = *targetPos_ - transform_.translate;
	transform_.rotate = Math::LookRotation(forward, { 0.0f, 1.0f, 0.0f });*/
}
