#include "BossHitReactionState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include "MoveBossState.h"

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossHitReactionState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossHitReactionState::Update() {
	// コンテキストの準備
	BossHitReactionComponent::UpdateContext context{
		.currentPosition = boss_->GetTransform().translate,
		.currentVelocity = boss_->GetVelocity(),
		.currentColor = boss_->GetColor(),
		.deltaTime = boss_->GetDeltaTime()
	};
	// 移動コンポーネントの更新
	BossHitReactionComponent::UpdateResult result = boss_->GetHitReactionComponent().Update(context);

	// 結果の適用
	boss_->SetVelocity(result.velocity);

	// カラーの更新
	boss_->SetColor(result.color);

	/// ===Stateの変更=== ///
	/// ===ヒットリアクションが終了したらMoveStateへ遷移=== ///
	float slowdownTimer = boss_->GetHitReactionComponent().GetKnockBackState().slowdownTimer;
	float colorTimer = boss_->GetHitReactionComponent().GetColorState().colorTimer;
	if (slowdownTimer <= 0.0f && colorTimer <= 0.0f) {
		boss_->ChangeState(std::make_unique<MoveBossState>());
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossHitReactionState::Finalize() {
	BossState::Finalize();
}