#include "EnemyTeleportState.h"
// MobEnemy
#include "application/Game/Entity/Enemy/MobEnemy/Base/MobEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include <Service/Particle.h>
// State
#include "EnemyMoveState.h"
#include "EnemyPrePareAttackState.h"

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
EnemyTeleportState::EnemyTeleportState(const float minRange, const float maxRange) {
	minRange_ = minRange;
	maxRange_ = maxRange;
}

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void EnemyTeleportState::Enter(MobEnemy* enemy) {
	enemy_ = enemy;
	enemy_->SetVelocity({ 0.0f, 0.0f, 0.0f });

	// テレポート開始処理
	enemy_->GetTeleportComponent().Start(
		enemy_->GetTransform().translate,
		enemy_->GetTransform().rotate,
		enemy_->GetPlayer()->GetTransform().translate, 
		minRange_, maxRange_);

	// Enemyの当たり判定を一時的に無効化
	enemy_->SetColliderActive(false);
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void EnemyTeleportState::Update() {
	// コンテキストの準備
	EnemyTeleportComponent::UpdateContext context{
		.currentPosition = enemy_->GetTransform().translate,
		.currentRotation = enemy_->GetTransform().rotate,
		.playerPosition = enemy_->GetPlayer()->GetTransform().translate,
		.deltaTime = enemy_->GetDeltaTime(),
	};
	// テレポートコンポーネントの更新
	EnemyTeleportComponent::UpdateResult result = enemy_->GetTeleportComponent().Update(context);

	/// ===結果の適用=== ///
	// 移動ベクトルの適用
	enemy_->SetVelocity(result.velocity);

	// 回転の反映
	enemy_->SetRotate(result.rotation);

	// 透明度の反映
	Vector4 currentColor = enemy_->GetColor();
	enemy_->SetColor({ currentColor.x, currentColor.y, currentColor.z, result.colorAlpha });

	// 向き補間（nextPosition 方向を向く）
	if (result.needsRotation) {
		float rotationSpeed = enemy_->GetTeleportComponent().GetConfig().rotationSpeed;
		enemy_->UpdateRotationTowards(result.targetDirection, rotationSpeed);
	}

	// 回転完了フラグのリセット
	if (result.shouldResetRotationFlag) {
		enemy_->SetIsRotationComplete(false);
	}

	// パーティクルのスポーン
	if (result.shouldSpawnParticleAtCurrent || result.shouldSpawnParticleAtNext) {
		// パーティクルをスポーン
		teleportParticle_ = Service::Particle::Emit("MobEnemyTeleport", enemy_->GetTransform().translate);
	}

	/// ===Stateの変更=== ///
	if (result.isComplete) {
		// Colliderを再度有効化
		enemy_->SetColliderActive(true);
		if (enemy_->CheckAttackable() && enemy_->GetAttackInfo().timer <= 0.0f && !enemy_->GetAttackInfo().isAttack) {
			// Attackに
			enemy_->ChangeState(std::make_unique<EnemyPrePareAttackState>());
		} else {
			// Moveに
			enemy_->ChangeState(std::make_unique<EnemyMoveState>());
		}
	}
}

///-------------------------------------------/// 
/// 終了時に呼びだす
///-------------------------------------------///
void EnemyTeleportState::Finalize() {
	EnemyState::Finalize();
}
