#include "BossOrbitingOrbsState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/BossMoveState.h>

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossOrbitingOrbsState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃開始
	boss_->GetOrbitingOrbsComponent().StartAttack();
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossOrbitingOrbsState::Update() {
	BossAttackOrbitingOrbsComponent::UpdateContext context{
		.bossPosition = boss_->GetTransform().translate,
		.deltaTime = boss_->GetDeltaTime(),
	};
	// AttackComponentを更新
	BossAttackOrbitingOrbsComponent::UpdateResult result = boss_->GetOrbitingOrbsComponent().Update(context);

	/// ===結果の反映=== ///
	// 攻撃終了判定
	if (result.isFinished) {
		// 次の状態へ遷移
		boss_->ChangeState(std::make_unique<BossMoveState>());
		return;
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossOrbitingOrbsState::Finalize() {
	boss_->GetAttackManager().StartOrbitingOrbsCooldown();

	BossState::Finalize();
}
