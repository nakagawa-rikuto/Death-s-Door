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
// C++
#include <algorithm>
// Math
#include <Math/sMath.h>

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

	/// ===移動処理=== ///
	UpdateMove();

	/// ===波紋の生成=== ///
	boss_->GetGroundOcean()->AddRipple(boss_->GetTransform().translate, 1.0f, 0.1f);
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossMoveState::Finalize() {
	BossState::Finalize();
}

///-------------------------------------------/// 
/// 移動処理を更新する。
///-------------------------------------------///
void BossMoveState::UpdateMove() {
	// コンポーネントのパラメータを取得
	BossComponent::MoveComponent component = boss_->GetComponentParameters().move;

	// プレイヤーへのベクトルを計算
	Vector3 toPlayer = boss_->GetPlayer()->GetTransform().translate - boss_->GetTransform().translate;
	// 高低差を無視
	toPlayer.y = 0.0f;

	// プレイヤーに向かってゆっくり移動する方向を計算
	if (Length(toPlayer) > 0.001f) {
		state_.direction = Normalize(toPlayer);
	}

	// 速度の更新
	Vector3 velocity = state_.direction * component.moveSpeed;
	velocity.y = boss_->GetVelocity().y; // Y軸の速度は維持
	boss_->SetVelocity(velocity);

	// プレイヤーの方を徐々に向くように回転を計算
	const Vector3 forward = { 0.0f, 0.0f, 1.0f }; // ボスの正面方向（Z軸）
	const Quaternion targetRotate = Math::DirectionToQuaternion(forward, state_.direction);
	const float t = (std::min)(component.rotationSpeed * boss_->GetDeltaTime(), 1.0f);
	Quaternion rotate = Math::SLerp(boss_->GetTransform().rotate, targetRotate, t);
	boss_->SetRotate(rotate);

}

///-------------------------------------------/// 
/// Stateの変更が必要か判定して、必要なら変更する。
///-------------------------------------------///
bool BossMoveState::ChangeStateIfNeeded(float dist) {

	/// ===遷移状態の判定=== ///
	if (boss_->CanRotateAttack(dist)) {
		// Rotate攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossRotateAttackState>());
		return true;
	} else if (boss_->CanDownSwingAttack(dist)) {
		// Downswing攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossDownwarAttackState>());
		return true;
	} else if (boss_->CanParabolicShot(dist)) {
		// ParabolicShot攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossParabolicShotState>());
		return true;
	} else if (boss_->CanOrbitingOrbs(dist)) {
		// OrbitingOrbs攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossOrbitingOrbsState>());
		return true;
	} else if (boss_->CanJumpSmashAttack(dist)) {
		// JumpSmash攻撃へ遷移
		boss_->ChangeState(std::make_unique<BossJumpSmashAttackState>());
		return true;
	}

	return false;
}
