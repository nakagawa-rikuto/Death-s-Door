#include "BossParabolicShotState.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// BulletManager
#include "application/Game/Entity/Enemy/BossEnemy/Bullet/BossBulletManager.h"
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
	// コンポーネントの取得
	auto& comp = boss_->GetAttackComponentManager().GetParabolicShotComponent();
	// コンテキストの準備
	BossAttackParabolicShotComponent::UpdateContext context{
		.bulletPosition = boss_->GetBulletManager().GetParabolicBulletPosition(),
		.bossPosition = boss_->GetTransform().translate,
		.targetPosition = boss_->GetPlayer()->GetTransform().translate,
		.deltaTime = boss_->GetDeltaTime()
	};
	BossAttackParabolicShotComponent::UpdateResult result = comp.Update(context);

	// 弾の位置を反映
	if (result.isFlying) {
		boss_->GetBulletManager().SetParabolicVelocity(result.velocity);
	}

	// 回転を適用
	if (comp.isLockOn()) {
		// Plaeyrの方向に回転させる
		//boss_->SetRotate(Math::LookRotation(result.faceDirection));
	}

	// 地面に当たったか
	if (comp.IsHitGround()) {
		// 波紋の生成
		boss_->GetGroundOcean()->AddRipple(comp.GetHitPosition(), 1.0f, 1.0f);
		// Bulletの削除
		boss_->GetBulletManager().KillLatestParabolicBullets();
	}

	/// ===Stateの移動=== ///
	if (result.isFinished) {
		// Bulletの削除
		boss_->GetBulletManager().KillLatestParabolicBullets();
		// リセット
		comp.Reset();
		// 次の状態へ遷移
		boss_->ChangeState(std::make_unique<BossMoveState>());
		return;
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossParabolicShotState::Finalize() {
	boss_->GetAttackComponentManager().StartParabolicShotCooldown(); // クールダウン開始
}
