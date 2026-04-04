#include "MoveState.h"
// Service
#include "Service/Input.h"
#include "Service/Particle.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Camera
#include "Engine/Camera/Base/CameraCommon.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include "RootState.h"
#include "AvoidanceState.h"
#include "AttackState.h"
// Math
#include "Math/sMath.h"


///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void MoveState::Enter(Player* player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	// タイマーのリセット
	rippleTimer_ = 0.0f;
	rippleTime_ = 0.01f;

	// 移動パーティクルの強制停止
	if (moveParticle_) {
		moveParticle_->Stop();
		moveParticle_ = nullptr;
	}
	// 移動パーティクルの再生
	moveParticle_ = Service::Particle::Emit("PlayerWarke", player_->GetTransform().translate);
	moveParticle_->SetEmitterPosition(player_->GetTransform().translate);
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void MoveState::Update(Player * player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	/// ===スティックの取得=== ///
	Vector2 leftStick = player_->GetLeftStickState();
	bool hasInput = std::abs(leftStick.x) > 0.1f || std::abs(leftStick.y) > 0.1f;
	if (hasInput) {
		// スティックの動きを適用
		ApplyStickMovement(leftStick);

	} else {
		// 入力がない場合は減速処理を適用
		ApplyBraking();
	}

	// 波紋を出す
	player_->GetGroundOcean()->AddRipple(player_->GetTransform().translate, 0.5f, 0.01f);

	// パーティクルの位置更新
	if (moveParticle_) {
		moveParticle_->SetEmitterPosition(player_->GetTransform().translate);
	}

	/// ===Stateの変更=== ///
	// 攻撃ボタンが押されたら攻撃状態へ
	if (Service::Input::TriggerButton(0, ControllerButtonType::X)) {
		// 攻撃の準備ができていれば
		if (!player_->GetAttackComponent()->GetState().isActive) {
			// AttackStateへ移行
			player_->ChangState(std::make_unique<AttackState>());
		}

	// Aボタンが押されたら回避状態へ
	} else if (Service::Input::TriggerButton(0, ControllerButtonType::A)) {
		// 回避の準備ができていれば
		if (player_->GetAvoidanceComponent()->GetState().isPreparation) {
			// AvoidanceStateへ移行
			player_->ChangState(std::make_unique<AvoidanceState>(Normalize(player_->GetMoveComponent()->GetCurrentDirection())));
		}

	// 移動が無ければ通常状態へ
	} else if (std::abs(player_->GetVelocity().x) <= 0.01f && std::abs(player_->GetVelocity().z) <= 0.01f) {
		// RootStateへ移行
		player_->ChangState(std::make_unique<RootState>());
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void MoveState::Finalize() {
	StopMoveParticle();
	PlayerState::Finalize();
}

///-------------------------------------------/// 
/// パーティクルの停止
///-------------------------------------------///
void MoveState::StopMoveParticle() {
	if (moveParticle_) {
		moveParticle_->Stop();
		moveParticle_ = nullptr;
	}
}

///-------------------------------------------/// 
/// スティックの動きを適用します。
///-------------------------------------------///
void MoveState::ApplyStickMovement(const Vector2& stick) {
	/// ===移動処理=== ///
	PlayerMoveComponent::UpdateContext context{
		.inputDirection = { stick.x, 0.0f, stick.y},
		.currentPosition = player_->GetTransform().translate,
		.currentRotation = player_->GetTransform().rotate,
		.deltaTime = player_->GetDeltaTime()
	};
	// 更新
	auto result = player_->GetMoveComponent()->Update(context);

	// 結果を反映
	result.velocity.y = player_->GetVelocity().y; // 現在のY軸の速度を維持
	player_->SetVelocity(result.velocity);
	player_->SetRotate(result.targetRotation);
}

///-------------------------------------------/// 
/// 減速処理
///-------------------------------------------///
void MoveState::ApplyBraking() {
	/// ===減速処理(数値を下げるほどゆっくり止まる)=== ///
	player_->ApplyDeceleration(player_->GetMoveComponent()->GetConfig().deceleration);
}
