#include "RootState.h"
// Service
#include "Engine/System/Service/InputService.h"
// Player, Camera
#include "application/Game/Entity/Player/Player.h"
// State
#include "MoveState.h"
#include "AvoidanceState.h"
#include "ChargeState.h"
#include "AttackState.h"

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void RootState::Enter(Player* player, GameCamera* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void RootState::Update(Player * player, GameCamera* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	/// ===左スティックの取得=== ///
	StickState leftStick = InputService::GetLeftStickState(0);

	/// ===減速処理(数値を下げるほどゆっくり止まる)=== ///
	player_->ApplyDeceleration(deceleration_);

	/// ===Stateの変更=== ///
	// RBボタンが押されたら進んでいる突進状態へ
	if (InputService::TriggerButton(0, ControllerButtonType::RB)) {
		// タイマーがクールタイムより高ければ、
		if (player_->GetpreparationFlag(actionType::kCharge)) {
			player_->ChangState(std::make_unique<ChargeState>(Normalize(player_->GetVelocity())));
		}
	// Aボタンが押されたら回避状態へ
	} else if (InputService::TriggerButton(0, ControllerButtonType::A)) {
		// 回避の準備ができていれば
		if (player_->GetpreparationFlag(actionType::kAvoidance)) {
			player_->ChangState(std::make_unique<AvoidanceState>(Normalize(player_->GetVelocity())));
		}
	}
	// 移動が有れば
	if (std::abs(leftStick.x) > 0.1f || std::abs(leftStick.y) > 0.1f) {
		// Stateを移動状態へ
		player_->ChangState(std::make_unique<MoveState>());
	}
}

///-------------------------------------------/// 
/// 終了時に呼びだす
///-------------------------------------------///
void RootState::Finalize() {
	PlayerState::Finalize();
}
