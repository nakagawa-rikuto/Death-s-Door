#include "EnemyMoveState.h"
// BaseEnemy
#include "application/Game/Entity/Enemy/Base/BaseEnemy.h"
// State
#include "EnemyAttackState.h"


///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void EnemyMoveState::Enter(BaseEnemy* enemy) {
	enemy_ = enemy;
	// 移動範囲の中心を設定
	enemy_->CommonMoveInit();
	// 動くまでの時間を設定
	enemy_->SetTimer(StateType::Move, 0.5f);
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void EnemyMoveState::Update(BaseEnemy * enemy) {
	enemy_ = enemy;

	enemy_->CommonMove();

	/// ===Stateの変更=== ///
	if (enemy_->CheckAttackable() && enemy_->GetAttackTimer() <= 0.0f && !enemy_->GetAttackFlag()) {
		// Attackに
		enemy_->ChangeState(std::make_unique<EnemyAttackState>());
	}
}

///-------------------------------------------/// 
/// 終了時に呼びだす
///-------------------------------------------///
void EnemyMoveState::Finalize() {
	EnemyState::Finalize();
}