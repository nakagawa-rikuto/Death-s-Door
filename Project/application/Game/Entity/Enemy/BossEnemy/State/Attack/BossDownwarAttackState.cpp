#include "BossDownwarAttackState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/MoveBossState.h>

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossDownwarAttackState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃開始
	boss_->GetDownswingComponent().StartAttack();
	// 武器を有効化
	boss_->GetWeapon().SetActive(true); 
}

///-------------------------------------------///  
/// 更新時に呼び出す
///-------------------------------------------///
void BossDownwarAttackState::Update() {
	// コンテキストの準備
	BossAttackDownwardSwingComponent::UpdateContext context{
		.baseRotation = boss_->GetTransform().rotate,
		.deltaTime = boss_->GetDeltaTime(),
	};
	// AttackComponentを更新
	BossAttackDownwardSwingComponent::UpdateResult result = boss_->GetDownswingComponent().Update(context);

	/// ===結果の反映=== ///
	// 速度の反映
	boss_->SetVelocity(result.velocity);

	// 回転の反映
	boss_->SetRotate(result.rotation);

	// 武器のオフセットを反映
	boss_->GetWeapon().SetTranslate(result.weaponPosition);

	if (result.isFinished) {

		// 攻撃終了 → MoveBossStateへ遷移
		boss_->ChangeState(std::make_unique<MoveBossState>());
		return;
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossDownwarAttackState::Finalize() {
	// タイマーリセット
	boss_->SetDownSwingTimer(boss_->GetAttackInfo().downswingCooldown);

	// 武器を無効化
	boss_->GetWeapon().SetActive(false);

	BossState::Finalize();
}

///-------------------------------------------/// 
/// プレイヤーとボスの距離を計算して返す。
///-------------------------------------------///
float BossDownwarAttackState::CalcDistToPlayer() const {
	return 0.0f;
}
