#pragma once
/// ===Include=== ///
#include <application/Game/Entity/Enemy/BossEnemy/State/Base/BossState.h>

///=====================================================/// 
/// BossOrbitingOrbsState
///=====================================================///
class BossOrbitingOrbsState : public BossState  {
public:
	~BossOrbitingOrbsState() override = default;

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
	// 弱い参照でBossEnemyを持つ
	BossEnemy* boss_;

	bool hasOrbited_ = false;

};

