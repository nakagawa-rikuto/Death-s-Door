#include "BossOrbitingOrbsState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Move/BossMoveState.h>

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossOrbitingOrbsState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 現在のスケールを保存
	state_.scale_ = boss_->GetTransform().scale; 

	// フェーズをAnticipationに設定
	currentPhase_ = Phase::Anticipation;
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossOrbitingOrbsState::Update() {
	/// ===フェーズがAnticipation(予備動作)のとき=== ///
	if (currentPhase_ == Phase::Anticipation) {
		// タイマーを進める
		state_.anticipationTimer += boss_->GetDeltaTime();

		// 予備動作の時間に応じてスケールを変化させる
		float t = state_.anticipationTimer / anticipationInfo_.time;
		if (t > 1.0f) t = 1.0f;
		float easing = (std::sin(t * Math::Pi() * 4.0f) + 1.0f) * 0.5f;
		float scale = Math::Lerp(anticipationInfo_.minScale, anticipationInfo_.maxScale, easing);

		boss_->SetScale({ scale, scale, scale });

		// 時間が経過したらフェーズ移行
		if (state_.anticipationTimer >= anticipationInfo_.time) {
			currentPhase_ = Phase::StartAttack;
		}
	}

	/// ===フェーズがStartAttackになったら=== ///
	if (currentPhase_ == Phase::StartAttack) {
		// 現在のスケールを取得
		Vector3 currentScale = boss_->GetTransform().scale;

		// スケールを自然に 1.0f に戻す
		float t = (std::min)(boss_->GetDeltaTime() * anticipationInfo_.returnSpeed, 1.0f);

		// Lerpを使ってスケールを徐々に 1.0f に近づける
		currentScale.x = Math::Lerp(currentScale.x, 1.0f, t);
		currentScale.y = Math::Lerp(currentScale.y, 1.0f, t);
		currentScale.z = Math::Lerp(currentScale.z, 1.0f, t);
		boss_->SetScale(currentScale);

		// スケールがほぼ 1.0f に戻りきったら攻撃し、次のステートへ遷移
		if (std::abs(currentScale.x - 1.0f) <= 0.01f) {

			// スケールを完全に 1.0f にリセット
			boss_->SetScale({ 1.0f, 1.0f, 1.0f });

			// 攻撃の開始
			boss_->GetBulletManager().SpawnOrbitingBullets(boss_->GetTransform().translate);

			// MoveStateに遷移
			boss_->ChangeState(std::make_unique<BossMoveState>());
			return;
		}
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossOrbitingOrbsState::Finalize() {
	// クールダウンの設定
	boss_->SetOrbitingOrbsCooldown();

	BossState::Finalize();
}