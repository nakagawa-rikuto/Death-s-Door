#include "EnemyAttackState.h"
// BaseEnemy
#include "application/Game/Entity/Enemy/Base/BaseEnemy.h"
#include "application/Game/Entity/Enemy/LongRange/LongRangeEnemy.h"
// State
#include "EnemyMoveState.h"


///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void EnemyAttackState::Enter(BaseEnemy* enemy) {
	enemy_ = enemy;
	// 移動量を初期化
	enemy_->SetVelocity({ 0.0f, 0.0f, 0.0f });
	// 攻撃タイマーを設定
	enemy_->SetTimer(StateType::Attack, 1.0f);
	// 色の設定
	enemy_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	// 回転差分を揃える
	if (auto* lr = dynamic_cast<LongRangeEnemy*>(enemy_)) {
		lr->SetlastYaw();
	}
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void EnemyAttackState::Update(BaseEnemy * enemy) {
	enemy_ = enemy;
	// preフラグに入れる
	preIsAttack_ = enemy_->GetAttackFlag();

	// Attackの処理
	enemy_->Attack();

	/// ===Stateの変更=== ///
	// preフラグがtrueでisAttackがfalseの時
	if (!enemy_->GetAttackFlag() && preIsAttack_) {
		enemy_->ChangeState(std::make_unique<EnemyMoveState>());
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void EnemyAttackState::Finalize() {
	EnemyState::Finalize();
}
