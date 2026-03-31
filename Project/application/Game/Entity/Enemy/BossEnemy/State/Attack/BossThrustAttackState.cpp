#include "BossThrustAttackState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/MoveBossState.h>

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossThrustAttackState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃開始
	boss_->GetThrustComponent().StartAttack();
	boss_->GetWeapon().SetActive(true); // 武器を有効化
}

///-------------------------------------------///  
/// 更新時に呼び出す
///-------------------------------------------///
void BossThrustAttackState::Update() {
	// コンテキストの準備
	BossAttackThrustComponent::UpdateContext context{
		.baseRotation = boss_->GetTransform().rotate,
		.deltaTime = boss_->GetDeltaTime(),
	};
	// AttackComponentを更新
	BossAttackThrustComponent::UpdateResult result = boss_->GetThrustComponent().Update(context);

	// 結果の反映
	boss_->SetRotate(result.modelRotation);

	// 武器のオフセットを反映
	boss_->GetWeapon().SetTranslate(result.weaponLocalOffset);

	if (result.isFinished) {
		// 攻撃終了 → MoveBossStateへ遷移
		boss_->ChangeState(std::make_unique<MoveBossState>());
		return;
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossThrustAttackState::Finalize() {
	// タイマーリセット
	boss_->SetThrustTimer(boss_->GetAttackInfo().thrustCooldown);

	// 武器を無効化
	boss_->GetWeapon().SetActive(false);

	BossState::Finalize();
}

///-------------------------------------------/// 
/// プレイヤーとボスの距離を計算して返す。
///-------------------------------------------///
float BossThrustAttackState::CalcDistToPlayer() const {
	return 0.0f;
}
