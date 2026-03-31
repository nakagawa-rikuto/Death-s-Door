#include "BossTeleportState.h"
// Boss
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include <Service/Particle.h>
// State
#include <application/Game/Entity/Enemy/BossEnemy/State/Attack/BossJumpSmashAttackState.h>

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
BossTeleportState::BossTeleportState(const float minRange, const float maxRange) {
	minRange_ = minRange;
	maxRange_ = maxRange;
}

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossTeleportState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	teleportCount_ = 0; // テレポート回数の初期化
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });

	// テレポート開始
	boss_->GetTeleportComponent().Start(
		boss_->GetTransform().translate,
		boss_->GetTransform().rotate,
		boss_->GetPlayer()->GetTransform().translate, 
		minRange_, maxRange_);

	// Bossの当たり判定を一時的に無効化
	boss_->SetColliderActive(false);
}
 
///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossTeleportState::Update() {
	if (!boss_) return;

	// コンテキストの準備
	BossTeleportComponent::UpdateContext context{
		.currentPosition = boss_->GetTransform().translate,
		.currentRotation = boss_->GetTransform().rotate,
		.playerPosition = boss_->GetPlayer()->GetTransform().translate,
		.deltaTime = boss_->GetDeltaTime(),
	};
	// テレポートコンポーネントの更新
	BossTeleportComponent::UpdateResult result = boss_->GetTeleportComponent().Update(context);

	/// ===結果の適用=== ///
	// 移動ベクトルの適用
	boss_->SetVelocity(result.velocity);

	// 回転の反映
	boss_->SetRotate(result.rotation);

	// 透明度の反映
	Vector4 currentColor = boss_->GetColor();
	boss_->SetColor({ currentColor.x, currentColor.y, currentColor.z, result.colorAlpha });

	// 向き補間（nextPosition 方向を向く）
	if (result.needsRotation) {
		float rotationSpeed = boss_->GetTeleportComponent().GetConfig().rotationSpeed;
		boss_->UpdateRotationTowards(result.targetDirection, rotationSpeed);
	}

	// 回転完了フラグのリセット
	if (result.shouldResetRotationFlag) {
		boss_->SetIsRotationComplete(false);
	}

	// パーティクルのスポーン
	if (result.shouldSpawnParticleAtCurrent || result.shouldSpawnParticleAtNext) {
		//teleportParticle_ = Service::Particle::Emit("BossEnemyTeleport", boss_->GetTransform().translate);
	}

	/// ===Stateの変更=== ///
	if (result.isComplete) {
		teleportCount_++;

		if (teleportCount_ < kMaxTeleportCount) {
			// テレポートを繰り返す
			boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
			boss_->GetTeleportComponent().Start(
				boss_->GetTransform().translate,
				boss_->GetTransform().rotate,
				boss_->GetPlayer()->GetTransform().translate, 
				minRange_, maxRange_);
		} else {
			// Colliderを再度有効化
			boss_->SetColliderActive(true);

			// Stateを変更
			boss_->ChangeState(std::make_unique<BossJumpSmashAttackState>());
		}
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossTeleportState::Finalize() {
	BossState::Finalize();
}
