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
	// 攻撃の開始
	boss_->GetBulletManager().SpawnOrbitingBullets();
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossOrbitingOrbsState::Update() {
	// 次の状態へ遷移
	boss_->ChangeState(std::make_unique<BossMoveState>());
	return;
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossOrbitingOrbsState::Finalize() {
	// クールダウンの設定
	boss_->SetOrbitingOrbsCooldown();

	BossState::Finalize();
}