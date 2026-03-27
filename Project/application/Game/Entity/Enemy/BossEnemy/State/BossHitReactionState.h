#pragma once
/// ===Include=== ///
#include "Base/BossState.h"

///=====================================================/// 
/// BossHitReactionState
/// ヒットリアクションの状態
///=====================================================///
class BossHitReactionState : public BossState {
public:
	~BossHitReactionState() override = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="enemy">状態に入るボスエネミーへのポインタ。</param>
	void Enter(BossEnemy* enemy) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;
};

