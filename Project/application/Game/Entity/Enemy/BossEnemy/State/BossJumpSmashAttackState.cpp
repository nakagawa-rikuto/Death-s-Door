#include "BossJumpSmashAttackState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include "MoveBossState.h"

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossJumpSmashAttackState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃開始
	const float dist = CalcDistToPlayer();
	boss_->GetJumpSmashComponent().StartAttack(
		dist,
		boss_->GetTransform().translate, 
		boss_->GetPlayer()->GetTransform().translate, 
		boss_->GetTransform().rotate);

	// 武器を有効化
	boss_->GetWeapon().SetActive(true); 
}

///-------------------------------------------///  
/// 更新時に呼び出す
///-------------------------------------------///
void BossJumpSmashAttackState::Update() {
	// コンテキストの準備
	BossAttackJumpSmashComponent::UpdateContext context{
		.deltaTime = boss_->GetDeltaTime(),
	};
	// AttackComponentを更新
	BossAttackJumpSmashComponent::UpdateResult result = boss_->GetJumpSmashComponent().Update(context);

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
void BossJumpSmashAttackState::Finalize() {
	// タイマーリセット
	boss_->SetJumpSmashTimer(boss_->GetAttackInfo().jumpSmashCooldown);

	// 武器を無効化
	boss_->GetWeapon().SetActive(false);

	BossState::Finalize();
}

///-------------------------------------------/// 
/// プレイヤーとボスの距離を計算して返す。
///-------------------------------------------///
float BossJumpSmashAttackState::CalcDistToPlayer() const {
	const Vector3 diff =
		boss_->GetPlayer()->GetTransform().translate -
		boss_->GetTransform().translate;
	return Length(diff);
}
