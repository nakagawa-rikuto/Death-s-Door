#include "RootState.h"
// Service
#include "Service/Input.h"
// Player, Camera
#include "application/Game/Entity/Player/Player.h"
// State
#include "MoveState.h"
#include "AvoidanceState.h"
#include "AttackState.h"

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void RootState::Enter(Player* player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void RootState::Update(Player * player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	/// ===左スティックの取得=== ///
	StickState leftStick = Service::Input::GetLeftStickState(0);

	/// ===死亡していなければ=== ///
	if (!player_->GetIsDead()) {
		/// ===Stateの変更=== ///
		// 攻撃ボタンが押されたら攻撃状態へ
		if (Service::Input::TriggerButton(0, ControllerButtonType::X)) {
			// 攻撃の準備ができていれば
			if (!player_->GetAttackComponent()->GetState().isActive) {
				player_->ChangState(std::make_unique<AttackState>());
			}
			// RBボタンが押されたら進んでいる突進状態へ
		} else if (Service::Input::TriggerButton(0, ControllerButtonType::RB)) {

			// Aボタンが押されたら回避状態へ
		} else if (Service::Input::TriggerButton(0, ControllerButtonType::A)) {
			// 回避の準備ができていれば
			if (player_->GetAvoidanceComponent()->GetState().isPreparation) {
				// パーティクル発生
				player_->ChangState(std::make_unique<AvoidanceState>(Normalize(player_->GetVelocity())));
			}
			// 移動が有れば
		} else if (std::abs(leftStick.x) > 0.1f || std::abs(leftStick.y) > 0.1f) {
			// Stateを移動状態へ
			player_->ChangState(std::make_unique<MoveState>());
		}
	}
}

///-------------------------------------------/// 
/// 終了時に呼びだす
///-------------------------------------------///
void RootState::Finalize() {
	PlayerState::Finalize();
}
