#include "BossParabolicShotState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
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
	boss_->GetParabolicShotComponent().StartAttack(
		boss_->GetTransform().translate,
		boss_->GetPlayer()->GetTransform().translate,
		boss_->GetGroundYPos());
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossParabolicShotState::Update() {
	// コンテキストの準備
	BossAttackParabolicShotComponent::UpdateContext context{
		.bulletPosition = boss_->GetParabolicShotComponent().IsActive() ? boss_->GetParabolicShotComponent().GetState().velocity : Vector3{ 0.0f, 0.0f, 0.0f },
		.deltaTime = boss_->GetDeltaTime(),
	};
	// AttackComponentを更新
	BossAttackParabolicShotComponent::UpdateResult result = boss_->GetParabolicShotComponent().Update(context);

	/// ===結果を反映=== ///
	// 速度の反映



	// 攻撃終了の判定
	if (result.isFinished) {
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
