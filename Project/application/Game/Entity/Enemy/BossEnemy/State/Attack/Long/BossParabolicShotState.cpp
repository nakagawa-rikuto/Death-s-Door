#include "BossParabolicShotState.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// BulletManager
#include "application/Game/Entity/Enemy/BossEnemy/Bullet/BossBulletManager.h"
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/BossMoveState.h>
// C++
#include <algorithm>
// Math
#include <Math/sMath.h>


///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossParabolicShotState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// BulletManagerへの参照を取得
	bulletManager_ = &boss_->GetBulletManager();
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃開始
	StartAttack(boss_->GetGroundYPos());
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossParabolicShotState::Update() {

	/// ===攻撃の更新=== ///
	UpdateAttack();

	/// ===Stateの移動=== ///
	if (phase_ == Phase::Finished) {
		// Bulletの削除
		bulletManager_->KillLatestParabolicBullets();
		// リセット
		state_ = State{};
		// 次の状態へ遷移
		boss_->ChangeState(std::make_unique<BossMoveState>());
		return;
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossParabolicShotState::Finalize() {
	// クールダウンの設定
	boss_->SetParabolicShotCooldown();
	BossState::Finalize();
}

///-------------------------------------------/// 
/// 攻撃の開始処理
///-------------------------------------------///
void BossParabolicShotState::StartAttack(float groundPosY) {
	if (phase_ != Phase::Idle) return;

	// コンポーネントのパラメータを取得
	BossComponent::ParabolicShotAttackComponent component = boss_->GetComponentParameters().parabolicShotAttack;

	// 地面のY座標を保存
	groundYPos_ = groundPosY;
	// 時間を保存
	state_.lockOnTimer = component.trebleDuration;

	// フェーズをLockOnに変更
	phase_ = Phase::LockOn;
	isHitGround_ = false;

	// 弾の生存時間の計算
	float totalLifetime = state_.lifeTimer + component.trebleDuration;

	// BulletManagerに弾の生成を依頼
	bulletManager_->SpawnParabolicBullets(boss_->GetTransform().translate, totalLifetime);
}

///-------------------------------------------/// 
/// 攻撃の更新処理
///-------------------------------------------///
void BossParabolicShotState::UpdateAttack() {
	// 非アクティブ時は何もしない
	if (phase_ == Phase::Idle || phase_ == Phase::Finished) return;

	switch (phase_) {
	case BossParabolicShotState::Phase::LockOn:
		UpdateLockOn();
		break;
	case BossParabolicShotState::Phase::Flying:
		UpdateFlying();
		break;
	}
}

///-------------------------------------------/// 
/// ロックオンフェーズの更新処理
///-------------------------------------------///
void BossParabolicShotState::UpdateLockOn() {
	// コンポーネントのパラメータを取得
	BossComponent::ParabolicShotAttackComponent component = boss_->GetComponentParameters().parabolicShotAttack;

	// LockOn時間の経過
	state_.lockOnTimer -= boss_->GetDeltaTime();

	// プレイヤー方向に回転指せる
	Vector3 direction = Normalize(boss_->GetPlayer()->GetTransform().translate - boss_->GetTransform().translate);
	direction.y = 0.0f; // 水平面上の方向に限定
	const Vector3 forward = { 0.0f, 0.0f, 1.0f }; // ボスの正面方向（Z軸）
	const Quaternion targetRotate = Math::DirectionToQuaternion(forward, direction);
	const float t = (std::min)(component.rotationSpeed * boss_->GetDeltaTime(), 1.0f);
	state_.rotate = Math::SLerp(boss_->GetTransform().rotate, targetRotate, t);
	boss_->SetRotate(state_.rotate);

	// LockOn完了
	if (state_.lockOnTimer <= 0.0f) {
		state_.bulletVelocity = CalcInitialVelocity(boss_->GetTransform().translate, boss_->GetPlayer()->GetTransform().translate);
		initialSpeed_ = Length(state_.bulletVelocity);
		state_.lifeTimer = component.lifeTime;
		phase_ = Phase::Flying;
	}
}

///-------------------------------------------/// 
/// 飛行フェーズの更新処理
///-------------------------------------------///
void BossParabolicShotState::UpdateFlying() {

	// コンポーネントのパラメータを取得
	BossComponent::ParabolicShotAttackComponent component = boss_->GetComponentParameters().parabolicShotAttack;
	// deltaTimeの取得
	float deltaTime = boss_->GetDeltaTime();

	/// ===生存タイマー更新=== ///                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
	state_.lifeTimer -= deltaTime;

	/// ===速度の更新=== ///
	state_.bulletVelocity.y -= component.gravity * deltaTime;
	bulletManager_->SetParabolicVelocity(state_.bulletVelocity);

	/// ===着弾判定=== ///
	isHitGround_ = component.enableGroundHit &&
		(bulletManager_->GetParabolicBulletPosition().y <= groundYPos_) &&
		(state_.bulletVelocity.y < 0.0f); 
	// 着弾した場合の処理
	if (isHitGround_) {
		// 着弾位置の計算
		hitPosition_ = bulletManager_->GetParabolicBulletPosition();
		hitPosition_.y = groundYPos_;
		// 波紋の生成
		boss_->GetGroundOcean()->AddRipple(hitPosition_, 1.0f, 1.0f);
		// Bulletの削除
		bulletManager_->KillLatestParabolicBullets();
	}

	// 終了条件チェック
	if (state_.lifeTimer <= 0.0f || isHitGround_) {
		phase_ = Phase::Finished;
	}
}

///-------------------------------------------/// 
/// 発射初速ベクトルを計算する。
///-------------------------------------------///
Vector3 BossParabolicShotState::CalcInitialVelocity(const Vector3 & from, const Vector3 & to) const {
	// 角度をラジアンに変換するための定数
	const float kDegToRad = Math::Pi() / 180.0f;

	// コンポーネントのパラメータを取得
	BossComponent::ParabolicShotAttackComponent component = boss_->GetComponentParameters().parabolicShotAttack;

	// 水平方向のベクトルと距離
	const Vector3 horizontal = { to.x - from.x, 0.0f, to.z - from.z };
	const float   dx = Length(horizontal);
	const float   dy = to.y - from.y;
	// 発射角度の三角関数
	const float cosTheta = std::cos(component.launchAngle * kDegToRad);
	const float sinTheta = std::sin(component.launchAngle * kDegToRad);
	const float tanTheta = (cosTheta > 1e-6f) ? (sinTheta / cosTheta) : 1e6f;
	// 分母: dx*tanθ - dy
	const float denom = dx * tanTheta - dy;

	float v0 = 0.0f;
	if (dx > 1e-3f && denom > 1e-3f) {
		// 物理式から v0 を求める
		const float cos2 = cosTheta * cosTheta;
		const float numerator = component.gravity * dx * dx;
		const float denomFull = 2.0f * cos2 * denom;
		v0 = std::sqrt(numerator / denomFull);
	} else {
		// フォールバック: 水平速度上限を使用
		v0 = (dx > 1e-3f)
			? (component.maxHorizontalSpeed / (std::max)(cosTheta, 1e-3f))
			: component.maxHorizontalSpeed;
	}

	// 水平速度上限を適用（v0 * cosθ <= maxHorizontalSpeed）
	const float hSpeed = v0 * cosTheta;
	if (hSpeed > component.maxHorizontalSpeed) {
		v0 = component.maxHorizontalSpeed / (std::max)(cosTheta, 1e-3f);
	}

	// 水平方向の単位ベクトル
	const Vector3 hDir = (dx > 1e-6f)
		? Vector3{ horizontal.x / dx, 0.0f, horizontal.z / dx }
	: Vector3{ 0.0f, 0.0f, 1.0f };

	return Vector3{
		hDir.x * v0 * cosTheta,
		v0 * sinTheta,           // 上向き初速
		hDir.z * v0 * cosTheta
	};
}
