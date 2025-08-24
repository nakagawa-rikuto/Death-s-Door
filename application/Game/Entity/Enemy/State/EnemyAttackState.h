#pragma once
/// ===Include=== ///
#include "Base/EnemyState.h"

///=====================================================/// 
/// MoveState
///=====================================================///
class EnemyAttackState : public EnemyState {
public:

	~EnemyAttackState() override = default;

	// 状態に入ったときに呼ばれる
	void Enter(BaseEnemy* enemy) override;
	// 状態の更新処理
	void Update(BaseEnemy* enemy) override;
	// 終了処理はPlayerStateのFinalizeを呼び出す
	void Finalize() override;

private:

	bool preIsAttack_ = false;
};

