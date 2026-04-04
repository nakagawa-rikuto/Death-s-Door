#include "AttackState.h"
// Service
#include "Service/Input.h"
// Player
#include "application/Game/Entity/Player/Player.h"
#include "application/Game/Entity/Player/Weapon/PlayerWeapon.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include "RootState.h"
// Math
#include "Math/sMath.h"

///-------------------------------------------/// 
/// 状態に入ったときに呼ばれる
///-------------------------------------------///
void AttackState::Enter(Player* player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	// 攻撃の開始
	if (player_->GetAttackComponent()->StartAttack(0, player_->GetWeapon(), player_->GetRightHand(), player_->GetLeftHand())) {
		moveForwardStrength_ = 0.5f; // 通常攻撃の前方移動の強さ
		moveForwardOnAttackStart(); // 攻撃開始時に前方に移動
	}
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void AttackState::Update(Player* player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	PlayerAttackComponent* attackComp = player_->GetAttackComponent();
	// 早期リターン
	if (!attackComp) return;

	// 減速処理
	player_->ApplyDeceleration(0.4f);

	// 攻撃コンポーネントの更新
	attackComp->Update(player_->GetDeltaTime());

	// 攻撃ボタンが押されたらコンボを試行
	if (Service::Input::TriggerButton(0, ControllerButtonType::X)) {
		if (attackComp->CanCombo()) {
			if (attackComp->TryCombo(player_->GetWeapon(), player_->GetRightHand(), player_->GetLeftHand())) {
				moveForwardStrength_ = 2.5f; // コンボ攻撃の前方移動の強さ
				moveForwardOnAttackStart(); // 攻撃開始時に前方に移動
			}
		}
	}

	// 攻撃が終了したらRootStateへ遷移
	if (!attackComp->IsAttacking() && !attackComp->CanCombo()) {
		player_->ChangState(std::make_unique<RootState>());
	}
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void AttackState::Finalize() {
	PlayerState::Finalize();
}

///-------------------------------------------/// 
/// 攻撃開始時に前方に移動する処理
///-------------------------------------------///
void AttackState::moveForwardOnAttackStart() {
	// 見栄え向上のために少し攻撃方向に進む
	Vector3 forward = Math::RotateVector({ 0.0f, 0.0f, 1.0f }, player_->GetTransform().rotate);
	forward.y = 0.0f;
	if (forward.x != 0.0f || forward.z != 0.0f) {
		forward = Normalize(forward);
		player_->SetVelocity(forward * moveForwardStrength_);
	}
	// 波紋を出す
	player_->GetGroundOcean()->AddRipple(player_->GetTransform().translate, 1.0f, 0.05f);
}
