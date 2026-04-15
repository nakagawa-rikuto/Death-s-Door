#include "GameClearAnimation.h"
// Entity
#include "application/Game/Entity/Player/Player.h"
// GroundOcean
#include "application/Game/Object/GameGround/GroundOcean.h"
// Camera
#include "Engine/Camera/FollowCamera.h"
// Service
#include "Service/DeltaTime.h"
// Math
#include "Math/sMath.h"
#include "Math/EasingMath.h"
#include "Math/MatrixMath.h"

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void GameClearAnimation::Initialize(Player* player, MiiEngine::FollowCamera* camera) {
	player_ = player;
	camera_ = camera;

	// プレイヤーの回転を保存
	Quaternion playerRotation = player_->GetTransform().rotate;

	/// ===カメラ回転の初期設定=== ///
	cameraInfo_.timer = 0.0f;
	// プレイヤーの前方向ベクトルを取得
	cameraInfo_.playerForward = Math::TransformNormal({ 0.0f, 0.0f, 1.0f }, Math::MakeRotateQuaternionMatrix(playerRotation));
	// 開始位置のオフセット
	Vector3 startOffset = { 0.0f, 0.120f, -94.0f };
	// 終了位置のオフセット
	float finalDistance = 30.0f;
	float finalHeight = 0.08f;
	Vector3 endOffset = { 0.0f, -finalHeight, -finalDistance };
	// 開始角度と距離を計算
	cameraInfo_.startAngle = atan2f(startOffset.x, startOffset.z);
	cameraInfo_.distance = sqrtf(startOffset.x * startOffset.x + startOffset.z * startOffset.z);
	cameraInfo_.height = startOffset.y;
	// 終了角度を計算
	cameraInfo_.targetAngle = atan2f(endOffset.x, endOffset.z);
	// カメラの回転情報は画像から取得不可のため、現在の回転を使用
	cameraInfo_.startRotation = camera_->GetRotate();
	// 終了時の回転も現在の回転を維持
	cameraInfo_.targetRotation = { 0.0f, -0.98f, 0.2f, 0.0f };

	/// ===プレイヤー前進の初期設定=== ///
	advanceInfo_.timer = 0.0f;
	{
		// プレイヤーが向いている方向（Y成分を除いた水平方向）を取得
		Vector3 forward = cameraInfo_.playerForward;
		forward.y = 0.0f;

		// 水平成分で正規化
		float len = sqrtf(forward.x * forward.x + forward.z * forward.z);
		if (len > 0.0f) {
			forward.x /= len;
			forward.z /= len;
		}

		// プレイヤーの向きにY軸固定オフセット角を加算して前進方向を決定
		// kDirectionOffsetRad を変えることで方向を調整できる
		float offsetRad = PlayerAdvanceInfo::kDirectionOffsetRad;
		float cosA = cosf(offsetRad);
		float sinA = sinf(offsetRad);
		advanceInfo_.direction = {
			forward.x * cosA - forward.z * sinA,  // X成分をY軸回転
			0.0f,
			forward.x * sinA + forward.z * cosA   // Z成分をY軸回転
		};
	}

	/// ===カメラターゲット用の固定位置を登録=== ///
	// 初期化時のプレイヤー位置を固定値として保存し、以降カメラはこの位置を追跡する
	fixedCameraTargetPosition_ = player_->GetTransform().translate;

	/// ===カメラの設定=== ///
	auto followCamera = dynamic_cast<MiiEngine::FollowCamera*>(camera_);
	// 回転可能型に設定
	followCamera->SetFollowCamera(MiiEngine::FollowCameraType::Orbiting);
	// カメラのターゲットを固定位置に設定（プレイヤーではなく固定値のアドレスを渡す）
	followCamera->SetTarget(&fixedCameraTargetPosition_, &playerRotation);
	// 初期オフセットを設定
	followCamera->SetOrbitingOffset(startOffset);

	/// ===Playerの設定=== ///
	player_->SetGravity(-9.8f);
	// 初期速度をリセット
	player_->SetVelocity({ 0.0f, 0.0f, 0.0f });

	/// ===初期フェーズを設定=== ///
	currentPhase_ = ClearAnimationPhase::CameraRotation;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void GameClearAnimation::Update() {
	/// ===DeltaTimeの取得=== ///
	deltaTime_ = Service::DeltaTime::GetDeltaTime();

	/// ===フェーズごとの更新=== ///
	switch (currentPhase_) {
	case ClearAnimationPhase::CameraRotation:
		UpdateCameraRotation();
		break;
	case ClearAnimationPhase::PlayerAdvance:
		UpdatePlayerAdvance();
		break;
	case ClearAnimationPhase::Completed:
		// 完了後は何もしない
		break;
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void GameClearAnimation::Draw() {}

///-------------------------------------------/// 
/// 完了フラグの取得
///-------------------------------------------///
bool GameClearAnimation::IsCompleted() const { return currentPhase_ == ClearAnimationPhase::Completed; }

///-------------------------------------------/// 
/// カメラ回転フェーズの更新
///-------------------------------------------///
void GameClearAnimation::UpdateCameraRotation() {
	// タイマーを進める
	cameraInfo_.timer += deltaTime_;

	// 進行度を計算(0.0 ~ 1.0)
	float t = cameraInfo_.timer / cameraInfo_.duration;
	t = (std::min)(t, 1.0f);

	// Easingを適用（滑らかな回転）
	float easedT = Easing::EaseInOutCubic(t);

	// 現在の角度を補間
	float currentAngle = Math::Lerp(cameraInfo_.startAngle, cameraInfo_.targetAngle, easedT);

	// 距離と高さも補間
	float finalDistance = 30.0f;
	float finalHeight = 0.08f;
	float currentDistance = Math::Lerp(94.0f, finalDistance, easedT);
	float currentHeight = Math::Lerp(0.120f, -finalHeight, easedT);

	// 円周上のカメラ位置を計算
	Vector3 newOffset;
	newOffset.x = sinf(currentAngle) * currentDistance;
	newOffset.y = currentHeight;
	newOffset.z = cosf(currentAngle) * currentDistance;

	// カメラのオフセットを更新
	camera_->SetOrbitingOffset(newOffset);

	// カメラの回転を補間
	Quaternion currentRotation = Math::SLerp(cameraInfo_.startRotation, cameraInfo_.targetRotation, easedT);
	camera_->SetRotate(currentRotation);

	// 回転完了でプレイヤー前進フェーズへ
	if (cameraInfo_.timer >= cameraInfo_.duration) {
		currentPhase_ = ClearAnimationPhase::PlayerAdvance;
		advanceInfo_.timer = 0.0f;
		// 前進開始時にプレイヤーの重力を無効化
		player_->SetGravity(0.0f);
		player_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	}
}

///-------------------------------------------/// 
/// プレイヤー前進フェーズの更新
///-------------------------------------------///
void GameClearAnimation::UpdatePlayerAdvance() {
	// タイマーを進める
	advanceInfo_.timer += deltaTime_;

	// 進行度を計算(0.0 ~ 1.0)
	float t = advanceInfo_.timer / advanceInfo_.duration;

	if (t <= 1.0f) {
		// EaseInOutで滑らかに加減速
		float easedT = Easing::EaseInOutCubic(t);

		// 速度にイージングを適用
		float speedScale = sinf(easedT * Math::Pi());
		Vector3 velocity = {
			advanceInfo_.direction.x * advanceInfo_.speed * speedScale,
			0.0f,
			advanceInfo_.direction.z * advanceInfo_.speed * speedScale
		};
		player_->SetVelocity(velocity);
		player_->GetGroundOcean()->AddRipple(player_->GetTransform().translate, 0.5f, 0.01f);
	} else {
		// 前進完了：速度を止めてアニメーション終了
		player_->SetVelocity({ 0.0f, 0.0f, 0.0f });
		currentPhase_ = ClearAnimationPhase::Completed;
	}
}
