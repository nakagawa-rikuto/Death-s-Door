#pragma once
/// ===Include=== ///
#include "Base/BossState.h"

///=====================================================/// 
/// DownwarAttackState
/// 叩きつけ攻撃の状態を表すクラス
///=====================================================///
class BossDownwarAttackState : public BossState {
public:
	~BossDownwarAttackState() override = default;

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

private:
	/// <summary>
	/// プレイヤーとボスの距離を計算して返す。
	/// </summary>
	float CalcDistToPlayer() const;
};

