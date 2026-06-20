#include "BossRotateAttackState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/BossMoveState.h>
// Service
#include "Service/Particle.h"
// C++
#include <algorithm>
// Math
#include <Math/sMath.h>

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossRotateAttackState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃開始
	StartAttack();
	// Particleの生成
	Service::Particle::Emit("Boss", boss_->GetTransform().translate);
}

///-------------------------------------------///  
/// 更新時に呼び出す
///-------------------------------------------///
void BossRotateAttackState::Update() {

	/// ===時間の更新=== ///
	UpdateTimer();

	/// ===攻撃の更新=== ///
	UpdateAttack();

	/// ===結果の反映=== ///
	// 回転を反映
	boss_->SetRotate(state_.rotate);

	// 武器のオフセットを反映
	boss_->GetWeapon().SetTranslate(state_.weaponPosition);

	/// ===波紋の生成=== ///
	if (isAttacking_) {
		boss_->GetGroundOcean()->AddRipple(boss_->GetWeapon().GetWorldTranslate(), 0.5f, 0.1f);
	}

	/// ===Stateの変更=== ///
	if (isFinished_) {
		// BossMoveStateへ遷移
		boss_->ChangeState(std::make_unique<BossMoveState>());
		return;
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossRotateAttackState::Finalize() {
	// Particleの停止
	Service::Particle::StopParticle("Boss");

	// 武器を無効化
	boss_->GetWeapon().SetActive(false);

	// タイマーリセット
	boss_->SetRotateAttackCooldown();

	BossState::Finalize();
}

///-------------------------------------------/// 
/// 攻撃開始処理
///-------------------------------------------///
void BossRotateAttackState::StartAttack() {
	if (phase_ != Phase::Idle && phase_ != Phase::Finished) return;

	state_.phaseTimer = 0.0f;
	state_.rotate = boss_->GetTransform().rotate;
	phase_ = Phase::WindUp;
	isFinished_ = false;
}

///-------------------------------------------/// 
/// タイマーの更新
///-------------------------------------------///
void BossRotateAttackState::UpdateTimer() {
	/// ===タイマーの更新=== ///
	state_.phaseTimer += boss_->GetDeltaTime();
}

///-------------------------------------------/// 
/// 攻撃の更新を実行
///-------------------------------------------///
void BossRotateAttackState::UpdateAttack() {
	/// ===Phase毎の処理=== ///
	switch (phase_) {
	case BossRotateAttackState::Phase::WindUp:
		UpdateWindUp();
		break;
	case BossRotateAttackState::Phase::Strike:
		UpdateStrike();
		break;
	case BossRotateAttackState::Phase::Recovery:
		UpdateRecovery();
		break;
	}
}

///-------------------------------------------/// 
/// 予備動作の更新を実行
///-------------------------------------------///
void BossRotateAttackState::UpdateWindUp() {
	// t
	const float t = (boss_->GetComponentParameters().rotateAttack.windUpDuration > 0.0f) ? (std::min)(state_.phaseTimer / boss_->GetComponentParameters().rotateAttack.windUpDuration, 1.0f) : 1.0f;

	// フェーズ遷移
	if (t >= 1.0f) {
		// Particleの停止
		Service::Particle::StopParticle("Boss");
		// 武器を有効化
		boss_->GetWeapon().SetActive(true);
		// 攻撃フラグを立てる
		isAttacking_ = true;
		// 次のフェーズへ
		state_.phaseTimer = 0.0f;
		phase_ = Phase::Strike;
	}
}

///-------------------------------------------/// 
/// 攻撃の更新を実行
///-------------------------------------------///
void BossRotateAttackState::UpdateStrike() {
	// t
	const float t = (boss_->GetComponentParameters().rotateAttack.strikeDuration > 0.0f) ? (std::min)(state_.phaseTimer / boss_->GetComponentParameters().rotateAttack.strikeDuration, 1.0f) : 1.0f;

	// Y軸ひねり
	const Quaternion strikeYaw = Math::RotateY(boss_->GetComponentParameters().rotateAttack.strikeAngle * (Math::Pi() / 180.0f));
	state_.rotate = Multiply(boss_->GetTransform().rotate, strikeYaw);

	// 武器オフセット
	state_.weaponPosition = boss_->GetComponentParameters().rotateAttack.weaponOffset;

	// フェーズ遷移
	if (t >= 1.0f) {
		state_.phaseTimer = 0.0f;
		phase_ = Phase::Recovery;
	}

}

///-------------------------------------------/// 
/// 回復の更新を実行
///-------------------------------------------///
void BossRotateAttackState::UpdateRecovery() {
	// t
	const float t = (boss_->GetComponentParameters().rotateAttack.recoveryDuration > 0.0f) ? (std::min)(state_.phaseTimer / boss_->GetComponentParameters().rotateAttack.recoveryDuration, 1.0f) : 1.0f;

	// 回転の補間
	const Quaternion strikeEndYaw = Math::RotateY(-boss_->GetComponentParameters().rotateAttack.strikeAngle * (Math::Pi() / 180.0f));
	const Quaternion strikeEndRot = Multiply(boss_->GetTransform().rotate, strikeEndYaw);
	state_.rotate = Math::SLerp(strikeEndRot, boss_->GetTransform().rotate, t);

	// 武器オフセット
	state_.weaponPosition = boss_->GetComponentParameters().rotateAttack.weaponOffset;

	// フェーズ遷移
	if (t >= 1.0f) {
		// 攻撃終了
		isAttacking_ = false;
		phase_ = Phase::Finished;
		isFinished_ = true;
	}
}
