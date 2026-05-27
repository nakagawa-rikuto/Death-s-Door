#include "BossParabolicShotState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// AttackComponentManager
#include "application/Game/Entity/Enemy/BossEnemy/Component/Attack/BossAttackComponentManager.h"
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
	boss_->GetAttackComponentManager().StartParabolicShot(boss_->GetTransform().translate, boss_->GetGroundYPos());
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossParabolicShotState::Update() {

	auto& comp = boss_->GetAttackComponentManager().GetParabolicShotComponent();

	// 回転を適用
	/*if (comp.isLockOn()) {
		boss_->SetRotate(Math::LookRotation(comp.Get()));
	}*/

	/// ===Stateの移動=== ///
	if (comp.IsHitGround()) {
		// 波紋の生成
		boss_->GetGroundOcean()->AddRipple(comp.GetHitPosition(), 1.0f, 1.0f);

		// 次の状態へ遷移
		boss_->ChangeState(std::make_unique<BossMoveState>());
		return;
	}

	
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossParabolicShotState::Finalize() {

}
