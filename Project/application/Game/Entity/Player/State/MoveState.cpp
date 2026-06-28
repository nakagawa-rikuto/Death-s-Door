#include "MoveState.h"
// Service
#include "Service/Input.h"
#include "Service/Particle.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include "RootState.h"
#include "DodgeState.h"
#include "AttackState.h"
// Math
#include "Math/sMath.h"
// C++
#include <algorithm>

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void MoveState::Enter(Player* player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	// 状態の初期化
	state_.direction = { 0.0f, 0.0f, 0.0f };
	state_.velocity = { 0.0f, 0.0f, 0.0f };
	state_.rotation = player_->GetTransform().rotate;

	// 移動パーティクルの強制停止
	if (moveParticle_) {
		moveParticle_->Stop();
		moveParticle_ = nullptr;
	}
	// 移動パーティクルの再生
	Vector3 particlePosition = player_->GetTransform().translate - Vector3{ 0.0f, player_->GetOBB().halfSize.y, 0.0f };
	moveParticle_ = Service::Particle::Emit("PlayerMove", particlePosition);
	moveParticle_->SetEmitterPosition(player_->GetTransform().translate);
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void MoveState::Update() {

	/// ===スティックの取得=== ///
	Vector2 leftStickInput_ = player_->GetLeftStickValue();
	bool hasInput = std::abs(leftStickInput_.x) > 0.1f || std::abs(leftStickInput_.y) > 0.1f;
	if (hasInput) {
		// スティックの動きを適用
		ApplyStickMovement(leftStickInput_);

	} else {
		// 入力がない場合は減速処理を適用
		PlayerState::ApplyDeceleration(player_->GetParameters().move.deceleration);
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
		/// AttackStateへ移行
		player_->ChangState(std::make_unique<AttackState>());

	// Aボタンが押されたら回避状態へ
	} else if (Service::Input::TriggerButton(0, ControllerButtonType::A)) {
		// DodgeStateへ移行
		if (player_->CanDodge()) {
			player_->ChangState(std::make_unique<DodgeState>(Normalize(state_.direction)));
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
	// 状態の更新
	state_.direction = { stick.x, 0.0f, stick.y };
	state_.velocity = state_.direction * player_->GetParameters().move.speed;

	// 回転
	if (Length(state_.direction) > 0.0f) {
		// 現在のYaw角
		float currentYaw = Math::GetYAngle(state_.rotation);
		// 目標のYaw角
		float targetYaw = std::atan2(state_.direction.x, state_.direction.z);
		// 差分を [-π, π] に正規化
		float diff = Math::NormalizeAngle(targetYaw - currentYaw);
		// イージング補間（短い方向へ回転）
		float easedYaw = currentYaw + diff * (player_->GetDeltaTime() * 10.0f);
		// Quaternionに再変換
		state_.rotation = Math::MakeRotateAxisAngle({ 0, 1, 0 }, easedYaw);
	} else {
		// 入力がない場合は現在の回転を維持
		state_.rotation = player_->GetTransform().rotate;
	}

	// 結果を反映
	state_.velocity.y = player_->GetVelocity().y; // 現在のY軸の速度を維持
	player_->SetVelocity(state_.velocity);
	player_->SetRotate(state_.rotation);
}

///-------------------------------------------/// 
/// キーボード入力に基づいて移動処理を行います。
///-------------------------------------------///
void MoveState::MoveKeyboard() {

}
