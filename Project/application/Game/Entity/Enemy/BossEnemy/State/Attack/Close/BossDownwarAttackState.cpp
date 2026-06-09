#include "BossDownwarAttackState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/BossMoveState.h>
// C++
#include <algorithm>
// Math
#include <Math/sMath.h>
#include <Math/EasingMath.h>

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossDownwarAttackState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃開始
	StartAttack(boss_->GetTransform().rotate);
	// 武器を有効化
	boss_->GetWeapon().SetActive(true); 
}

///-------------------------------------------///  
/// 更新時に呼び出す
///-------------------------------------------///
void BossDownwarAttackState::Update() {
	
	/// ===タイマーの更新=== ///
	UpdateTimer();

	/// ===攻撃の更新=== ///
	UpdateAttack();

	/// ===状態の反映=== ///
	// 速度の反映
	boss_->SetVelocity(state_.velocity);

	// 回転の反映
	boss_->SetRotate(state_.rotation);

	// 武器のオフセットを反映
	boss_->GetWeapon().SetTranslate(state_.weaponPosition);

	/// ===Stateの遷移=== ///
	if (isFinished_) {
		// BossMoveStateへ遷移
		boss_->ChangeState(std::make_unique<BossMoveState>());
		return;
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossDownwarAttackState::Finalize() {
	// タイマーリセット
	boss_->SetDownwardSwingAttackCooldown();

	// 武器を無効化
	boss_->GetWeapon().SetActive(false);

	BossState::Finalize();
}

///-------------------------------------------/// 
/// 攻撃の開始処理
///-------------------------------------------///
void BossDownwarAttackState::StartAttack(const Quaternion& rotate) {
	// フェーズがIdleかFinished以外のときは開始できない
	if (phase_ != Phase::Idle && phase_ != Phase::Finished) return;

	state_.phaseTimer = 0.0f;
	startRotation = rotate;
	phase_ = Phase::WindUp;
	isFinished_ = false;
}

///-------------------------------------------/// 
/// タイマーの更新
///-------------------------------------------///
void BossDownwarAttackState::UpdateTimer() {
	/// ===タイマーの更新=== ///
	state_.phaseTimer += boss_->GetDeltaTime();
}

///-------------------------------------------/// 
/// 攻撃の更新
///-------------------------------------------///
void BossDownwarAttackState::UpdateAttack() {
	/// ===Phase毎の処理=== ///
	switch (phase_) {
	case BossDownwarAttackState::Phase::WindUp: 
		UpdateWindUp();
		break;
	case BossDownwarAttackState::Phase::Strike:
		UpdateStrike();
		break;
	case BossDownwarAttackState::Phase::HoldDown:
		UpdateHoldDown();
		break;
	case BossDownwarAttackState::Phase::Recovery:
		UpdateRecovery();
		break;
	}
}

///-------------------------------------------/// 
/// ワインドアップの更新
///-------------------------------------------///
void BossDownwarAttackState::UpdateWindUp() {
	// t
	const float rawT = (boss_->GetComponentParameters().downwardSwingAttack.windUpDuration > 0.0f) ? (std::min)(state_.phaseTimer / boss_->GetComponentParameters().downwardSwingAttack.windUpDuration, 1.0f) : 1.0f;
	const float t = Easing::EaseOutQuad(rawT);

	// 回転の補間
	const float currentAngle = Math::Lerp(0.0f, boss_->GetComponentParameters().downwardSwingAttack.windUpPitch, t);
	const Quaternion crouchRot = Math::RotateX(currentAngle * Math::Pi() / 180.0f);
	state_.rotation = Multiply(startRotation, crouchRot);
	boss_->SetRotate(state_.rotation);

	// 武器
	state_.weaponPosition = boss_->GetComponentParameters().downwardSwingAttack.weaponOffset;
	boss_->GetWeapon().SetTranslate(state_.weaponPosition);

	// 移動なし
	state_.velocity = { 0.0f, 0.0f, 0.0f };
	boss_->SetVelocity(state_.velocity);

	// フェーズ遷移
	if (rawT >= 1.0f) {
		// タイマーをリセット
		state_.phaseTimer = 0.0f;
		// ストライクへ遷移
		phase_ = Phase::Strike;
	}
}

///-------------------------------------------/// 
/// ストライクの更新
///-------------------------------------------///
void BossDownwarAttackState::UpdateStrike() {
	// t
	const float rawT = (boss_->GetComponentParameters().downwardSwingAttack.strikeDuration > 0.0f) ? (std::min)(state_.phaseTimer / boss_->GetComponentParameters().downwardSwingAttack.strikeDuration, 1.0f) : 1.0f;
	const float t = Easing::EaseInQuad(rawT);

	// 回転の補間
	const float startAngle = boss_->GetComponentParameters().downwardSwingAttack.windUpPitch;
	const float endAngle = boss_->GetComponentParameters().downwardSwingAttack.strikeForwardPitch;
	const float currentAngle = startAngle + (endAngle - startAngle) * t;
	const Quaternion strikePitch = Math::RotateX(currentAngle * Math::Pi() / 180.0f);
	state_.rotation = Multiply(boss_->GetTransform().rotate, strikePitch);

	// 武器
	state_.weaponPosition = boss_->GetComponentParameters().downwardSwingAttack.weaponOffset;

	// 踏み込み
	const float prevRawT = (std::max)(0.0f, (state_.phaseTimer - boss_->GetDeltaTime()) / boss_->GetComponentParameters().downwardSwingAttack.strikeDuration);
	const float prevT = Easing::EaseInQuad((std::min)(prevRawT, 1.0f));
	const float stepTotal = boss_->GetComponentParameters().downwardSwingAttack.strikeStepForward * t;
	const float prevStepTotal = boss_->GetComponentParameters().downwardSwingAttack.strikeStepForward * prevT;
	const float deltaStep = stepTotal - prevStepTotal;

	// baseRotation でローカル前方をワールド空間へ変換
	const Vector3 localForward = { 0.0f, 0.0f, 1.0f };
	const Vector3 worldForward = Math::RotateVector(localForward, boss_->GetTransform().rotate);
	// 前方単位ベクトル × 今フレームの移動量
	state_.velocity = Vector3{
		worldForward.x * deltaStep / boss_->GetDeltaTime(),
		worldForward.y * deltaStep / boss_->GetDeltaTime(),
		worldForward.z * deltaStep / boss_->GetDeltaTime()
	};

	// フェーズ遷移
	if (rawT >= 1.0f) {
		// タイマーをリセット
		state_.phaseTimer = 0.0f;
		// ホールドダウンの有無で遷移先を変える
		if (boss_->GetComponentParameters().downwardSwingAttack.holdDownDuration > 0.0f) {
			phase_ = Phase::HoldDown;
		} else {
			phase_ = Phase::Recovery;
		}
	}
}

///-------------------------------------------/// 
/// ホールドダウンの更新
///-------------------------------------------///
void BossDownwarAttackState::UpdateHoldDown() {
	// t
	const float t = (boss_->GetComponentParameters().downwardSwingAttack.holdDownDuration > 0.0f) ? (state_.phaseTimer / boss_->GetComponentParameters().downwardSwingAttack.holdDownDuration, 1.0f) : 1.0f;

	// 終了時点の値で固定
	const Quaternion holdPitch = Math::RotateX(-boss_->GetComponentParameters().downwardSwingAttack.strikeForwardPitch * Math::Pi() / 180.0f);

	// 回転はStrikeの終了時点の値で固定
	state_.rotation = Multiply(boss_->GetTransform().rotate, holdPitch);

	// 武器
	state_.weaponPosition = boss_->GetComponentParameters().downwardSwingAttack.weaponOffset;

	// 移動は止まる
	state_.velocity = Vector3{ 0.0f, 0.0f, 0.0f }; // 踏み込みは止まる

	// フェーズ遷移
	if (t >= 1.0f) {
		// 波紋を生成
		boss_->GetGroundOcean()->AddRipple(boss_->GetWeapon().GetWorldTranslate(), 1.0f, 50.0f);
		// タイマーリセット
		state_.phaseTimer = 0.0f;
		// リカバリーへ遷移
		phase_ = Phase::Recovery;
	}
}

///-------------------------------------------/// 
/// リカバリーの更新
///-------------------------------------------///
void BossDownwarAttackState::UpdateRecovery() {
	// t
	const float t = (boss_->GetComponentParameters().downwardSwingAttack.recoveryDuration > 0.0f) ? (std::min)(state_.phaseTimer / boss_->GetComponentParameters().downwardSwingAttack.recoveryDuration, 1.0f) : 1.0f;

	// 回転の補間 
	const Quaternion strikeEndRot = Multiply(boss_->GetTransform().rotate, Math::RotateX(-boss_->GetComponentParameters().downwardSwingAttack.strikeForwardPitch * Math::Pi() / 180.0f));
	state_.rotation = Math::SLerp(strikeEndRot, startRotation, t);

	// 武器
	state_.weaponPosition = boss_->GetComponentParameters().downwardSwingAttack.weaponOffset;

	// Recovery中は移動しない
	state_.velocity = Vector3{ 0.0f, 0.0f, 0.0f };

	// フェーズ遷移
	if (t >= 1.0f) {
		phase_ = Phase::Finished;
		isFinished_ = true;
	}
}

