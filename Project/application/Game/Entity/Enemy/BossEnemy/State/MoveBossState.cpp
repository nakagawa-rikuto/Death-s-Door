#include "MoveBossState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include "BossAttackState.h"

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void MoveBossState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void MoveBossState::Update() {
	// プレイヤーとの距離を計算
	const float dist = CalcDistToPlayer();

	// コンテキストの準備
	BossMoveComponent::UpdateContext context{
		.currentPosition = boss_->GetTransform().translate,
		.currentRotation = boss_->GetTransform().rotate,
		.playerPosition = boss_->GetPlayer()->GetTransform().translate,
		.deltaTime = boss_->GetDeltaTime(),
	};
	// 移動コンポーネントの更新
	BossMoveComponent::UpdateResult result = boss_->GetMoveComponent().Update(context);

	// 結果の適用
	result.velocity.y = boss_->GetVelocity().y;
	boss_->SetVelocity(result.velocity);

	// 回転の更新
	boss_->SetRotate(result.rotate);

	/// ===Stateの変更=== ///
	if (boss_->GetAttackComponent().IsAnyAttackAvailable(dist)) {
		boss_->ChangeState(std::make_unique<BossAttackState>());
		return;
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void MoveBossState::Finalize() {
	BossState::Finalize();
}

///-------------------------------------------/// 
/// プレイヤーとボスの距離を計算して返す。
///-------------------------------------------///
float MoveBossState::CalcDistToPlayer() const {
	const Vector3 diff =
		boss_->GetPlayer()->GetTransform().translate -
		boss_->GetTransform().translate;
	return Length(diff);
}
