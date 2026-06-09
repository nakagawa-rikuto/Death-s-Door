#include "BossJumpSmashAttackState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/BossMoveState.h>// C++
// C++
#include <algorithm>
// Math
#include <Math/sMath.h>
#include <Math/EasingMath.h>

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossJumpSmashAttackState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃開始
	const float dist = CalcDistToPlayer();
	StartAttack(dist);
	// 武器を有効化
	boss_->GetWeapon().SetActive(true); 
}

///-------------------------------------------///  
/// 更新時に呼び出す
///-------------------------------------------///
void BossJumpSmashAttackState::Update() {
	// 毎フレーム速度をリセット
	state_.velocity = { 0.0f, 0.0f, 0.0f }; 

	// 非アクティブフェーズ
	if (phase_ == Phase::Idle || phase_ == Phase::Finished) {
		state_.rotation = startRotation_;
		state_.weaponPosition = boss_->GetComponentParameters().jumpSmashAttack.weaponOffset;
		isFinished_ = (phase_ == Phase::Finished);
	} else {
		/// ===タイマーの更新=== ///
		state_.phaseTimer += boss_->GetDeltaTime();

		/// ===攻撃の更新=== ///
		UpdateAttack();
	}

	/// ===結果の反映=== ///
	// 速度の反映
	boss_->SetVelocity(state_.velocity);

	// 回転の反映
	boss_->SetRotate(state_.rotation);

	// 武器のオフセットを反映
	boss_->GetWeapon().SetTranslate(state_.weaponPosition);

	// 攻撃終了の判定
	if (isFinished_) {
		//  BossMoveStateへ遷移
		boss_->ChangeState(std::make_unique<BossMoveState>());
		return;
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossJumpSmashAttackState::Finalize() {
	// タイマーリセット
	boss_->SetJumpSmashAttackCooldown();

	// 武器を無効化
	boss_->GetWeapon().SetActive(false);

	BossState::Finalize();
}

///-------------------------------------------///
/// 攻撃開始
///-------------------------------------------///
void BossJumpSmashAttackState::StartAttack(float distance) {
	// すでにアクティブな場合は無視
	if (phase_ != Phase::Idle && phase_ != Phase::Finished) return;

	// 攻撃開始に必要な情報を state_ に保存
	state_.phaseTimer = 0.0f;
	state_.distance = std::clamp(
		distance, 
		boss_->GetComponentParameters().jumpSmashAttack.minDistance, 
		boss_->GetComponentParameters().jumpSmashAttack.maxDistance);
	state_.position = boss_->GetTransform().translate;

	startPosition_ = boss_->GetTransform().translate;
	targetPosition_ = boss_->GetPlayer()->GetTransform().translate;
	startRotation_ = boss_->GetTransform().rotate;

	phase_ = Phase::LeapWindUp;
}

///-------------------------------------------///
/// 攻撃の更新を実行（フェーズのディスパッチ）
///-------------------------------------------///
void BossJumpSmashAttackState::UpdateAttack() {
	/// ===Phase毎の処理=== ///
	switch (phase_) {
	case Phase::LeapWindUp: 
		UpdateLeapWindUp(); 
		break;
	case Phase::Leap:      
		UpdateLeap();      
		break;
	case Phase::Strike:     
		UpdateStrike();     
		break;
	case Phase::HoldDown:   
		UpdateHoldDown();   
		break;
	case Phase::Recovery:  
		UpdateRecovery();   
		break;
	default: break;
	}
}

///-------------------------------------------///
/// LeapWindUp（跳躍前の溜め）
///-------------------------------------------///
void BossJumpSmashAttackState::UpdateLeapWindUp() {
	// t
	const float rawT = (boss_->GetComponentParameters().jumpSmashAttack.leapWindUpDuration > 0.0f) ? (std::min)(state_.phaseTimer / boss_->GetComponentParameters().jumpSmashAttack.leapWindUpDuration, 1.0f) : 1.0f;
	const float t = Easing::EaseOutQuad(rawT);

	// 回転の補間
	const float crouchAngle = Math::Lerp(0.0f, boss_->GetComponentParameters().jumpSmashAttack.leapWindUpCrouchPitch, t);
	const Quaternion crouchRot = Math::RotateX(crouchAngle * (Math::Pi() / 180.0f));
	state_.rotation = Multiply(startRotation_, crouchRot);

	// 武器
	state_.weaponPosition = boss_->GetComponentParameters().jumpSmashAttack.weaponOffset;
	// 移動なし
	state_.velocity = {};

	// フェーズ遷移
	if (rawT >= 1.0f) {
		// 波紋を生成
		boss_->GetGroundOcean()->AddRipple(boss_->GetTransform().translate, 1.0f, 0.1f);
		// 次のフェーズへ
		state_.phaseTimer = 0.0f;
		phase_ = Phase::Leap;
	}
}

///-------------------------------------------///
/// Leap（放物線飛行）
///-------------------------------------------///
void BossJumpSmashAttackState::UpdateLeap() {
	// t
	float ration = state_.distance / boss_->GetComponentParameters().jumpSmashAttack.maxDistance;
	float actualDuration = boss_->GetComponentParameters().jumpSmashAttack.leapDuration * ration;
	const float t = (actualDuration > 0.0f) ? (std::min)(state_.phaseTimer / actualDuration, 1.0f) : 1.0f;

	/// ===位置計算=== ///
	// 水平
	const Vector3 horizontalPos = Math::Lerp(startPosition_, targetPosition_, t);
	// 垂直
	const float arcY = CalcParabolaHeight(t);
	const Vector3 nextPosition = Vector3{
		horizontalPos.x,
		horizontalPos.y + arcY,
		horizontalPos.z
	};
	// velocity
	state_.velocity = nextPosition - state_.position;

	// currentPositionを更新更新
	state_.position = nextPosition;

	/// ===ピッチ計算=== ///
	float pitchAngle = 0.0f;
	if (t <= 0.5f) {
		// 上昇フェーズ
		const float ascT = t / 0.5f; // 0.0〜1.0 に正規化
		pitchAngle = boss_->GetComponentParameters().jumpSmashAttack.leapAscentPitch * ascT;
	} else {
		// 降下フェーズ
		const float descT = (t - 0.5f) / 0.5f; // 0.0〜1.0 に正規化
		const float startAngle = boss_->GetComponentParameters().jumpSmashAttack.leapAscentPitch;
		const float endAngle = -boss_->GetComponentParameters().jumpSmashAttack.leapDescentPitch;
		pitchAngle = startAngle + (endAngle - startAngle) * descT;
	}
	const Quaternion leapPitch = Math::RotateX(pitchAngle * (Math::Pi() / 180.0f));
	state_.rotation = Multiply(startRotation_, leapPitch);

	/// ===武器オフセット=== ///
	if (t <= 0.5f) { // 上昇中
		const float weaponT = t / 0.5f;
		state_.weaponPosition = Math::Lerp(
			boss_->GetComponentParameters().jumpSmashAttack.weaponOffset, 
			boss_->GetComponentParameters().jumpSmashAttack.weaponOffset, weaponT);

	} else { // 降下中
		state_.weaponPosition = boss_->GetComponentParameters().jumpSmashAttack.weaponOffset;
	}

	// フェーズ遷移
	if (t >= 1.0f) {
		// 波紋を生成
		boss_->GetGroundOcean()->AddRipple(boss_->GetTransform().translate, 1.0f, 0.1f);
		boss_->GetGroundOcean()->AddRipple(boss_->GetWeapon().GetWorldTranslate(), 1.5f, 0.5f);
		// 着地時は targetPosition にスナップして誤差を除去
		state_.position = targetPosition_;
		// 次のフェーズへ
		state_.phaseTimer = 0.0f;
		phase_ = Phase::Strike;
	}
}

///-------------------------------------------///
/// Strike（着地叩きつけ）
///-------------------------------------------///
void BossJumpSmashAttackState::UpdateStrike() {

	// t
	const float rawT = (boss_->GetComponentParameters().jumpSmashAttack.strikeDuration > 0.0f) ? (std::min)(state_.phaseTimer / boss_->GetComponentParameters().jumpSmashAttack.strikeDuration, 1.0f) : 1.0f;
	const float t = Easing::EaseInQuad(rawT);

	// 回転の補間
	const float startAngle = -boss_->GetComponentParameters().jumpSmashAttack.leapDescentPitch;
	const float endAngle = boss_->GetComponentParameters().jumpSmashAttack.strikeForwardPitch;
	const float currentAngle = startAngle + (endAngle - startAngle) * t;
	const Quaternion strikePitch = Math::RotateX(currentAngle * (Math::Pi() / 180.0f));
	state_.rotation = Multiply(startRotation_, strikePitch);

	// 武器
	state_.weaponPosition = Math::Lerp(
		boss_->GetComponentParameters().jumpSmashAttack.weaponOffset, 
		boss_->GetComponentParameters().jumpSmashAttack.weaponOffset, t);

	// 着地済みのため移動なし
	state_.velocity = {};

	// フェーズ遷移
	if (rawT >= 1.0f) {
		state_.phaseTimer = 0.0f;
		if (boss_->GetComponentParameters().jumpSmashAttack.holdDownDuration > 0.0f) {
			phase_ = Phase::HoldDown;
		} else {
			phase_ = Phase::Recovery;
		}
	}
}

///-------------------------------------------///
/// HoldDown（叩きつけ余韻）
///-------------------------------------------///
void BossJumpSmashAttackState::UpdateHoldDown() {
	// t
	const float t = (boss_->GetComponentParameters().jumpSmashAttack.holdDownDuration > 0.0f) ? (std::min)(state_.phaseTimer / boss_->GetComponentParameters().jumpSmashAttack.holdDownDuration, 1.0f) : 1.0f;

	// 終了時点で固定
	const Quaternion holdPitch = Math::RotateX(boss_->GetComponentParameters().jumpSmashAttack.strikeForwardPitch * (Math::Pi() / 180.0f));
	state_.rotation = Multiply(startRotation_, holdPitch);
	state_.weaponPosition = boss_->GetComponentParameters().jumpSmashAttack.weaponOffset;
	state_.velocity = {};

	// フェーズ遷移
	if (t >= 1.0f) {
		state_.phaseTimer = 0.0f;
		phase_ = Phase::Recovery;
	}
}

///-------------------------------------------///
/// Recovery（戻り）
///-------------------------------------------///
void BossJumpSmashAttackState::UpdateRecovery() {
	// t
	const float t = (boss_->GetComponentParameters().jumpSmashAttack.recoveryDuration > 0.0f) ? (std::min)(state_.phaseTimer / boss_->GetComponentParameters().jumpSmashAttack.recoveryDuration, 1.0f) : 1.0f;

	// 回転の補間
	const Quaternion strikeEndRot = Multiply(startRotation_, Math::RotateX(boss_->GetComponentParameters().jumpSmashAttack.strikeForwardPitch * (Math::Pi() / 180.0f)));
	state_.rotation = Math::SLerp(strikeEndRot, startRotation_, t);

	// 武器
	state_.weaponPosition = Math::Lerp(
		boss_->GetComponentParameters().jumpSmashAttack.weaponOffset, 
		boss_->GetComponentParameters().jumpSmashAttack.weaponOffset, t);

	// velocity
	state_.velocity = {};

	// フェーズ遷移
	if (t >= 1.0f) {
		phase_ = Phase::Finished;
		isFinished_ = true;
	}
}

///-------------------------------------------///
/// 放物線の高さを計算
///-------------------------------------------///
float BossJumpSmashAttackState::CalcParabolaHeight(float t) const {
	const float ct = std::clamp(t, 0.0f, 1.0f);
	return boss_->GetComponentParameters().jumpSmashAttack.leapArcHeight * 4.0f * ct * (1.0f - ct);
}
