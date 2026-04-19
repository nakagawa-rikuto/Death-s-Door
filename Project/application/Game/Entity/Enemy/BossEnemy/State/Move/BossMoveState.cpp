#include "BossMoveState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Attack/Close/BossRotateAttackState.h>
#include <application/Game/Entity/Enemy/BossEnemy/State/Attack/Close/BossDownwarAttackState.h>
#include <application/Game/Entity/Enemy/BossEnemy/State/Attack/Close/BossJumpSmashAttackState.h>
#include <application/Game/Entity/Enemy/BossEnemy/State/Attack/Long/BossOrbitingOrbsState.h>
#include <application/Game/Entity/Enemy/BossEnemy/State/Attack/Long/BossParabolicShotState.h>
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
	if (ChangeStateIfNeeded(dist)) {
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

///-------------------------------------------/// 
/// Stateの変更が必要か判定して、必要なら変更する。
///-------------------------------------------///
bool BossMoveState::ChangeStateIfNeeded(float dist) {

	// 突き攻撃への遷移条件
	if (boss_->GetAttackManager().CanRotate(dist)) {
		// 突き攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossRotateAttackState>());
		return true;
		// Downswing攻撃への遷移条件
	} else if (boss_->GetAttackManager().CanDownswing(dist)) {
		// Downswing攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossDownwarAttackState>());
		return true;
		// OrbitingOrbs攻撃への遷移条件

	} else if (boss_->GetAttackManager().CanOrbitIngOrbs(dist)) {
		// OrbitingOrbs攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossOrbitingOrbsState>());
		return true;
	} else if (boss_->GetAttackManager().CanParabolicShot(dist)) {
		// ParabolicShot攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossParabolicShotState>());
		return true;
	} else if (boss_->GetAttackManager().CanJumpSmash(dist)) {
		// JumpSmash攻撃への遷移条件
		float minRange = boss_->GetAttackManager().GetConfig().jumpSmashMinRange;
		float maxRange = boss_->GetAttackManager().GetConfig().jumpSmashMaxRange;
		// テレポートへ遷移
		boss_->ChangeState(std::make_unique<BossTeleportState>(minRange, maxRange));
		return true;
	}

	return false;
}
