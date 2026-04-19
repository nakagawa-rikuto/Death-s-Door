#include "BossParabolicShotState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// AttackManager
#include "application/Game/Entity/Enemy/BossEnemy/Component/Attack/BossAttackManager.h"
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/BossMoveState.h>

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossParabolicShotState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃開始
	boss_->GetAttackManager().StartParabolicShot(
		boss_->GetTransform().translate,
		boss_->GetPlayer()->GetTransform().translate,
		boss_->GetGroundYPos());
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossParabolicShotState::Update() {
	// 次の状態へ遷移
	boss_->ChangeState(std::make_unique<BossMoveState>());
	return;
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossParabolicShotState::Finalize() {

}
