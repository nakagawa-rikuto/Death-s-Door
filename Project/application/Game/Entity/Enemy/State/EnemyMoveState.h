#pragma once
/// ===Include=== ///
#include "Base/EnemyState.h"
// Math
#include "Math/Vector3.h"

///=====================================================/// 
/// MoveState
///=====================================================///
class EnemyMoveState : public EnemyState {
public:

	~EnemyMoveState() override = default;

	// 状態に入ったときに呼ばれる
	void Enter(BaseEnemy* enemy) override;
	// 状態の更新処理
	void Update(BaseEnemy* enemy) override;
	// 終了処理はPlayerStateのFinalizeを呼び出す
	void Finalize() override;
};

