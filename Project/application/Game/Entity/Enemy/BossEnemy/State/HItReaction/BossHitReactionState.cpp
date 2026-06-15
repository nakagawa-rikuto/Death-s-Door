#include "BossHitReactionState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/BossMoveState.h>
// Math
#include <Math/sMath.h>
// C++
#include <algorithm>
#include <numbers>

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
BossHitReactionState::BossHitReactionState(const Vector3& hitDirection) {
	// 方向の取得
	hitDirection_ = hitDirection;
}

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossHitReactionState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// フラグを立てる
	isHit_ = true;
	// タイマーの初期化
	state_.slowdownTimer = boss_->GetComponentParameters().hitReaction.slowdownDuration;
	state_.colorTimer = boss_->GetComponentParameters().hitReaction.alphaDuration;
	state_.colorInterval = 0.0f;
	// 色変化フラグを立てる
	isColorChange_ = true;
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossHitReactionState::Update() {

	/// ===タイマーの更新=== ///
	UpdateTimer(boss_->GetDeltaTime());

	/// ===減速係数の更新=== ///
	UpdateSlowdownMultiplier();

	/// ===ノックバック処理=== ///
	UpdateKnockBack();
	boss_->SetVelocity(state_.velocity);

	/// ===回転の更新=== ///
	UpdateRotation();

	/// ===色の更新=== ///
	Vector4 color = UpdateColor(boss_->GetColor());
	boss_->SetColor(color);

	/// ===Stateの変更=== ///
	if (state_.slowdownTimer <= 0.0f && state_.colorTimer <= 0.0f) {
		boss_->ChangeState(std::make_unique<BossMoveState>());
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossHitReactionState::Finalize() {
	BossState::Finalize();
}

///-------------------------------------------/// 
/// タイマーの更新処理
///-------------------------------------------///
void BossHitReactionState::UpdateTimer(const float deltaTime) {
	// 減速タイマーの更新
	if (state_.slowdownTimer > 0.0f) {
		state_.slowdownTimer -= deltaTime;

		if (state_.slowdownTimer < 0.0f) {
			state_.slowdownTimer = 0.0f;
		}
	}

	// 色変化タイマーの更新
	if (state_.colorTimer > 0.0f) {
		state_.colorTimer -= deltaTime;
		state_.colorInterval += deltaTime;

		if (state_.colorTimer < 0.0f) {
			state_.colorTimer = 0.0f;
			state_.colorInterval = 0.0f;
			isColorChange_ = false;
		}
	}
}

///-------------------------------------------/// 
/// 回転の更新処理
///-------------------------------------------///
void BossHitReactionState::UpdateRotation() {
	// 回転の更新（ヒット時ののけ反りから徐々にX,Z軸の回転を自然な状態(0.0f)へ戻す）
	Quaternion currentRotation = boss_->GetTransform().rotate;
	Quaternion targetRotation = currentRotation;
	targetRotation.x = 0.0f;
	targetRotation.z = 0.0f;
	targetRotation = Normalize(targetRotation);

	float lerpT = (std::min)(1.0f, 10.0f * boss_->GetDeltaTime()); // 戻るスピード（10.0fは調整可能）
	currentRotation = Math::SLerp(currentRotation, targetRotation, lerpT);
	boss_->SetRotate(currentRotation);
}

///-------------------------------------------/// 
/// 減速係数の更新処理
///-------------------------------------------///
void BossHitReactionState::UpdateSlowdownMultiplier() {
	if (state_.slowdownTimer > 0.0f) {
		// 減速時間に応じて線形補間
		float t = state_.slowdownTimer / boss_->GetComponentParameters().hitReaction.slowdownDuration;
		state_.slowdownMultiplier = Math::Lerp(1.0f, boss_->GetComponentParameters().hitReaction.slowdownFactor, t);
	} else {
		// 減速時間が終了したら通常速度
		state_.slowdownMultiplier = 1.0f;
	}
}

///-------------------------------------------/// 
/// 渡された色を元に更新された色を返す処理
///-------------------------------------------///
Vector4 BossHitReactionState::UpdateColor(const Vector4& currentColor) const{
	Vector4 color = currentColor;

	if (isColorChange_ && state_.colorTimer > 0.0f) {
		// サイン波を使って点滅効果を生成
		float wave = std::sin(state_.colorInterval * boss_->GetComponentParameters().hitReaction.flashSpeed * 2.0f * std::numbers::pi_v<float>);

		// サイン波の値が正の時は半透明、負の時は通常
		if (wave > 0.0f) {
			color.w = boss_->GetComponentParameters().hitReaction.hitAlpha;
		} else {
			color.w = 1.0f;
		}
	} else {
		// 色変化が終了したら元の色に戻す
		color.w = 1.0f;
	}

	return color;
}

///-------------------------------------------/// 
/// ノックバックの更新処理
///-------------------------------------------///
void BossHitReactionState::UpdateKnockBack() {
	if (isHit_) {
		// ノックバック方向の計算
		Vector3 direction = hitDirection_;
		direction.y = 0.0f; // Y軸方向は無視

		// 正規化
		float length = Length(direction);
		if (length > 0.001f) {
			direction = Normalize(direction);
			// ノックバック速度を設定
			state_.velocity = direction * boss_->GetComponentParameters().hitReaction.knockBackForce;
		}

		// ヒットフラグをリセット
		isHit_ = false;
	} else {
		// ノックバック中でない場合は現在の速度に減速係数を適用
		state_.velocity = boss_->GetVelocity() * state_.slowdownMultiplier;
	}
}

