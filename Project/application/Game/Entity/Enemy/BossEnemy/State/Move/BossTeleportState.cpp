#include "BossTeleportState.h"
// Boss
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include <Service/Particle.h>
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Attack/Close/BossJumpSmashAttackState.h>
// Math
#include <Math/sMath.h>

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
BossTeleportState::BossTeleportState(const float minRange, const float maxRange) {
	minRange_ = minRange;
	maxRange_ = maxRange;

	// ランダムエンジンの初期化
	std::seed_seq seed{
		static_cast<uint32_t>(std::time(nullptr)),
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(this)),
	};
	randomEngine_.seed(seed);
}

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossTeleportState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	state_.teleportCount = 0; // テレポート回数の初期化
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });

	// テレポート開始
	StartTeleport();

	// Bossの当たり判定を一時的に無効化
	boss_->SetColliderActive(false);
}
 
///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossTeleportState::Update() {
	if (!boss_) return;

	/// ===テレポートの更新=== ///
	UpdateTeleport();

	/// ===結果の適用=== ///
	// 移動ベクトルの適用
	boss_->SetVelocity(state_.velocity);

	// 回転の反映
	boss_->SetRotate(state_.rotate);

	// 透明度の反映
	Vector4 currentColor = boss_->GetColor();
	boss_->SetColor({ currentColor.x, currentColor.y, currentColor.z, state_.colorAlpha });

	/// ===Stateの変更=== ///
	if (isFinished_) {
		state_.teleportCount++;

		if (state_.teleportCount < kMaxTeleportCount_) {
			// テレポートを繰り返す
			boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
			StartTeleport();
		} else {
			// Stateを変更
			boss_->ChangeState(std::make_unique<BossJumpSmashAttackState>());
		}
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossTeleportState::Finalize() {
	// Colliderを再度有効化
	boss_->SetColliderActive(true);
	BossState::Finalize();
}

///-------------------------------------------/// 
/// テレポート開始処理
///-------------------------------------------///
void BossTeleportState::StartTeleport() {
	// 状態のリセット
	phase_ = Phase::SpinOut;
	state_.phaseTimer = 0.0f;
	state_.colorAlpha = 1.0f;
	startRotation_ = boss_->GetTransform().rotate;
	state_.rotate = startRotation_;
	startPosition_ = boss_->GetTransform().translate;
	isFinished_ = false;

	// ワープ先の決定
	Vector3 offset = SetNextPosition(minRange_, maxRange_, startPosition_);
	nextPosition_ = offset + boss_->GetPlayer()->GetTransform().translate; // 現在位置からのオフセットとして計算
	nextPosition_.y = state_.rotate.y; // Y座標は変えない
}

///-------------------------------------------/// 
/// 次の位置を設定します
///-------------------------------------------///
Vector3 BossTeleportState::SetNextPosition(float minRange, float maxRange, const Vector3& currentPosition) {
	/// ===計算=== ///
	if (minRange > maxRange) {
		std::swap(minRange, maxRange);
	}

	// ランダムな角度と距離を生成
	std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * Math::Pi());
	std::uniform_real_distribution<float> distanceDist(minRange, maxRange);

	// ランダムな値の設定
	float angle = angleDist(randomEngine_);
	float distance = distanceDist(randomEngine_);

	// 移動先の設定
	return Vector3{
		std::cos(angle) * distance,
		currentPosition.y,
		std::sin(angle) * distance
	};
}

///-------------------------------------------/// 
/// テレポートを更新します
///-------------------------------------------///
void BossTeleportState::UpdateTeleport() {
	if (phase_ == Phase::None) return;

	/// ===フェーズごとの演出=== ///
	switch (phase_) {
	case BossTeleportState::Phase::SpinOut:
		UpdateSpinOut();
		break;
	case BossTeleportState::Phase::Warp:
		UpdateWarp();
		break;
	case BossTeleportState::Phase::SpinIn:
		UpdateSpinIn();
		break;
	}
}

///-------------------------------------------/// 
/// SpinOutフェーズの更新処理
///-------------------------------------------///
void BossTeleportState::UpdateSpinOut() {
	const float duration = boss_->GetComponentParameters().teleport.spinOutDuration;
	float t = (duration > 0.0f) ? (state_.phaseTimer / duration) : 1.0f;
	t = (std::min)(t, 1.0f);

	// 透明度を線形に下げる（1 → 0）
	state_.colorAlpha= 1.0f - t;

	// t² の EaseIn で 1周（2π）回転（startRotation を基点に乗算）
	const float spinAngle = (t * t) * 2.0f * Math::Pi();
	state_.rotate = Multiply(startRotation_, Math::RotateY(spinAngle));

	// 移動なし
	state_.velocity = { 0.0f, 0.0f, 0.0f };

	// 現在地にパーティクルを出し続ける
	Vector3 particlePosition = boss_->GetTransform().translate - Vector3{ 0.0f, boss_->GetOBB().halfSize.y, 0.0f };
	teleportParticle_ = Service::Particle::Emit("EnemyTeleportParticle", boss_->GetTransform().translate + boss_->GetOBB().halfSize);

	if (t >= 1.0f) {
		// 完全に透明になったら Warp フェーズへ
		startRotation_ = state_.rotate;
		phase_ = Phase::Warp;
		state_.phaseTimer = 0.0f;
	}
}

///-------------------------------------------/// 
/// Warpフェーズの更新処理
///-------------------------------------------///
void BossTeleportState::UpdateWarp() {
	const float duration = boss_->GetComponentParameters().teleport.warpDuration;

	// 完全に透明のまま
	state_.colorAlpha = 0.0f;

	// warpStartPosition → nextPosition を warpDuration 秒で均等に移動
	const float remaining = duration - (std::min)(state_.phaseTimer, duration);
	if (remaining > 0.0f) {
		Vector3 fullDistance = nextPosition_ - startPosition_;
		fullDistance.y = 0.0f;
		state_.velocity = fullDistance / duration;
	} else {
		state_.velocity = { 0.0f, 0.0f, 0.0f };
		// NextPosition に到達したら SpinIn へ
		startRotation_ = state_.rotate;
		phase_ = Phase::SpinIn;
		state_.phaseTimer = 0.0f;
	}
}

///-------------------------------------------/// 
/// SpinInフェーズの更新処理
///-------------------------------------------///
void BossTeleportState::UpdateSpinIn() {
	const float duration = boss_->GetComponentParameters().teleport.spinInDuration;
	float t = (duration > 0.0f) ? (state_.phaseTimer / duration) : 1.0f;
	t = (std::min)(t, 1.0f);

	// 透明度を線形に上げる（0 → 1）
	state_.colorAlpha = t;

	// √t の EaseOut で 1周（2π）回転（startRotation を基点に乗算）
	const float spinAngle = std::sqrt(t) * 2.0f * Math::Pi();
	state_.rotate = Multiply(startRotation_, Math::RotateY(spinAngle));

	// 移動なし
	state_.velocity = { 0.0f, 0.0f, 0.0f };

	// 現在地にパーティクル出し続ける
	Vector3 particlePosition = boss_->GetTransform().translate - Vector3{ 0.0f, boss_->GetOBB().halfSize.y, 0.0f };
	teleportParticle_ = Service::Particle::Emit("EnemyTeleportParticle", boss_->GetTransform().translate + boss_->GetOBB().halfSize);

	if (t >= 1.0f) {
		// 演出完了
		isFinished_ = true;
		phase_ = Phase::None;
	}
}
