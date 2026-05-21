#include "BossRotateAttackState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/BossMoveState.h>
// Service
#include "Service/Particle.h"

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossRotateAttackState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃開始
	boss_->GetRotateComponent().StartAttack();
	// Particleの生成
	Service::Particle::Emit("Boss", boss_->GetTransform().translate);
}

///-------------------------------------------///  
/// 更新時に呼び出す
///-------------------------------------------///
void BossRotateAttackState::Update() {
	// コンテキストの準備
	BossAttackRotateComponent::UpdateContext context{
		.baseRotation = boss_->GetTransform().rotate,
		.deltaTime = boss_->GetDeltaTime(),
	};
	// AttackComponentを更新
	BossAttackRotateComponent::UpdateResult result = boss_->GetRotateComponent().Update(context);

	// 結果の反映
	boss_->SetRotate(result.modelRotation);

	// 武器のオフセットを反映
	boss_->GetWeapon().SetTranslate(result.weaponLocalOffset);

	if (!result.isParticle) {
		Service::Particle::StopParticle("Boss");
		boss_->GetWeapon().SetActive(true); // 武器を有効化
	}

	// 波紋を生成するタイミングの判定
	if (result.isAttacking) {
		boss_->GetGroundOcean()->AddRipple(boss_->GetWeapon().GetWorldTranslate(), 0.5f, 0.1f);
	}

	if (result.isFinished) {
		// BossMoveStateへ遷移
		boss_->ChangeState(std::make_unique<BossMoveState>());
		return;
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossRotateAttackState::Finalize() {
	// タイマーリセット
	boss_->GetAttackManager().StartRotateCooldown();

	// 武器を無効化
	boss_->GetWeapon().SetActive(false);

	BossState::Finalize();
}

