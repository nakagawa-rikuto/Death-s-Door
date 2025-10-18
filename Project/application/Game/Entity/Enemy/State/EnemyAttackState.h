#pragma once
/// ===Include=== ///
#include "Base/EnemyState.h"

///=====================================================/// 
/// MoveState
///=====================================================///
class EnemyAttackState : public EnemyState {
public:

	~EnemyAttackState() override = default;

	/// <summary>
	/// ステートに入った時に呼ばれる処理
	/// </summary>
	/// <param name="enemy">処理対象の BaseEnemy オブジェクトへのポインタ。ステートへ入る対象となる敵を指す</param>
	void Enter(BaseEnemy* enemy) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="enemy">更新対象の BaseEnemy 型オブジェクトへのポインタ。</param>
	void Update(BaseEnemy* enemy) override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

private:

	bool preIsAttack_ = false;
};

