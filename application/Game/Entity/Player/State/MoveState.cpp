#include "MoveState.h"
// Service
#include "Engine/System/Service/InputService.h"
// Player, Camera
#include "application/Game/Entity/Player/Player.h"
#include "application/Game/Camera/GameCamera.h"
// State
#include "RootState.h"
#include "AvoidanceState.h"
#include "ChargeState.h"
#include "AttackState.h"
// Math
#include "Math/sMath.h"


///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void MoveState::Enter(Player* player, GameCamera* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	// 移動速度の設定
	info_.speed = 0.4f;
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void MoveState::Update(Player * player, GameCamera* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	/// ===左スティックの取得=== ///
	StickState leftStick = InputService::GetLeftStickState(0);

	/// ===移動処理=== ///
	info_.direction.x = leftStick.x;
	info_.direction.z = leftStick.y;

	/// ===Velocityに反映=== ///
	player_->SetVelocity(info_.direction * info_.speed);

	/// ===移動方向に沿って回転=== ///
	// 方向が変更されたら
	if (Length(info_.direction) > 0.01f) {
		// 現在のYaw角(Y軸の回転)を取得
		float currentYaw = Math::GetYAngle(player_->GetTransform().rotate);
		// 入力方向から目標のYaw角を取得
		float targetYaw = std::atan2(info_.direction.x, info_.direction.z);
		// 差分を [-π, π] に正規化
		float diff = Math::NormalizeAngle(targetYaw - currentYaw);
		// イージング補間（短い方向へ回転）
		float easedYaw = currentYaw + diff * (player_->GetDeltaTime() * 10.0f);
		// Quaternionに再変換
		player_->SetRotate(Math::MakeRotateAxisAngle({ 0, 1, 0 }, easedYaw));
	}

	/// ===Stateの変更=== ///
	// RBボタンが押されたら進んでいる突進状態へ
	if (InputService::TriggerButton(0, ControllerButtonType::RB)) {
		// タイマーがクールタイムより高ければ、
		if (player_->GetpreparationFlag(actionType::kCharge)) {
			player_->ChangState(std::make_unique<ChargeState>(Normalize(info_.direction)));
		}
	// Aボタンが押されたら回避状態へ
	} else if (InputService::TriggerButton(0, ControllerButtonType::A)) {
		// 回避の準備ができていれば
		if (player_->GetpreparationFlag(actionType::kAvoidance)) {
			player_->ChangState(std::make_unique<AvoidanceState>(Normalize(info_.direction)));
		}
	}
	// 移動があれば通常状態へ
	if (std::abs(leftStick.x) < 0.1f && std::abs(leftStick.y) < 0.1f) {
		player_->ChangState(std::make_unique<RootState>());
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void MoveState::Finalize() {
	PlayerState::Finalize();
}
