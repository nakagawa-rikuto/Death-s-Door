#include "BossMoveState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Attack/BossRotateAttackState.h>
#include <application/Game/Entity/Enemy/BossEnemy/State/Attack/BossDownwarAttackState.h>
#include <application/Game/Entity/Enemy/BossEnemy/State/Attack/BossJumpSmashAttackState.h>
#include "BossTeleportState.h"

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossMoveState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossMoveState::Update() {
	// プレイヤーとの距離を計算
	const float dist = CalcDistToPlayer();

	/// ===Stateの変更=== ///
	// 突き攻撃への遷移条件
	if (dist <= boss_->GetAttackInfo().thrustRange && boss_->GetAttackInfo().thrustTimer <= 0.0f) {
		// 突き攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossRotateAttackState>());
		return;
		// Downswing攻撃への遷移条件
	} else if (dist <= boss_->GetAttackInfo().downswingRange && boss_->GetAttackInfo().downswingTimer <= 0.0f) {
		// 突き攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossDownwarAttackState>());
		return;
		// JumpSmash攻撃への遷移条件
	} else if (boss_->GetAttackInfo().jumpSmashMinRange <= dist && dist <= boss_->GetAttackInfo().jumpSmashMaxRange && boss_->GetAttackInfo().jumpSmashTimer <= 0.0f) {
		float minRange = boss_->GetAttackInfo().jumpSmashMinRange;
		float maxRange = boss_->GetAttackInfo().jumpSmashMaxRange;
		// 突き攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossTeleportState>(minRange, maxRange));
		return;
	}

	// コンテキストの準備
	BossMoveComponent::UpdateContext context{
		.currentPosition = boss_->GetTransform().translate,
		.currentRotation = boss_->GetTransform().rotate,
		.playerPosition = boss_->GetPlayer()->GetTransform().translate,
		.deltaTime = boss_->GetDeltaTime(),
	};
	// 移動コンポーネントの更新
	BossMoveComponent::UpdateResult result = boss_->GetMoveComponent().Update(context);

	// 結果の適用
	result.velocity.y = boss_->GetVelocity().y;
	boss_->SetVelocity(result.velocity);

	// 回転の更新
	boss_->SetRotate(result.rotate);

	// 波紋の生成
	boss_->GetGroundOcean()->AddRipple(boss_->GetTransform().translate, 1.0f, 0.1f);
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossMoveState::Finalize() {
	BossState::Finalize();
}

///-------------------------------------------/// 
/// プレイヤーとボスの距離を計算して返す。
///-------------------------------------------///
float BossMoveState::CalcDistToPlayer() const {
	const Vector3 diff =
		boss_->GetPlayer()->GetTransform().translate -
		boss_->GetTransform().translate;
	return Length(diff);
}
