#include "EnemyMoveState.h"
// MobEnemy
#include "application/Game/Entity/Enemy/MobEnemy/Base/MobEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include "EnemyPrePareAttackState.h"
#include "EnemyTeleportState.h"


///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void EnemyMoveState::Enter(MobEnemy* enemy) {
	enemy_ = enemy;
	enemy_->SetVelocity({ 0.0f, 0.0f, 0.0f });
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void EnemyMoveState::Update() {
    // コンテキストの準備
    EnemyMoveComponent::UpdateContext context{
        .currentPosition = enemy_->GetTransform().translate,
		.playerPosition = enemy_->GetPlayer()->GetTransform().translate,
        .deltaTime = enemy_->GetDeltaTime(),
    };
    // 移動コンポーネントの更新
    EnemyMoveComponent::UpdateResult result = enemy_->GetMovementComponent().Update(context);

    // 結果の適用
    result.velocity.y = enemy_->GetVelocity().y;
    enemy_->SetVelocity(result.velocity);

	// 回転の更新
    float rotationSpeed = enemy_->GetMovementComponent().GetConfig().rotationSpeed;
    enemy_->UpdateRotationTowards(result.rotateDirection, rotationSpeed);

	/// ===Stateの変更=== ///
    if (result.teleportTrigger) {
        float minRange = 0.0f;
		float maxRange = 0.0f;
        if (enemy_->GetMovementComponent().GetState().isInEvadeRange) {
			// 回避範囲に入ったらテレポート
			minRange = enemy_->GetMovementComponent().GetConfig().evadeRange * 4.0f;
			maxRange = enemy_->GetMovementComponent().GetConfig().chaseRange;
        } else if (enemy_->GetMovementComponent().GetState().isInChaseRange) {
            // 追いかける範囲に入ったらテレポート
			minRange = enemy_->GetMovementComponent().GetConfig().evadeRange * 2.0f;
			maxRange = enemy_->GetMovementComponent().GetConfig().chaseRange;
        }
        // テレポートステートに
        enemy_->ChangeState(std::make_unique<EnemyTeleportState>(minRange, maxRange));

    } else if (enemy_->CheckAttackable() && enemy_->GetAttackInfo().timer <= 0.0f && !enemy_->GetAttackInfo().isAttack) {
        // Attackに
        enemy_->ChangeState(std::make_unique<EnemyPrePareAttackState>());
    }
}

///-------------------------------------------/// 
/// 終了時に呼びだす
///-------------------------------------------///
void EnemyMoveState::Finalize() {
	EnemyState::Finalize();
}