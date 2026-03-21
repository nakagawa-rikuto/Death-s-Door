#pragma once
/// ===Include=== ///
#include "Base/EnemyState.h"

///=====================================================/// 
/// TeleportState
///=====================================================///
class EnemyTeleportState : public EnemyState {
public:
	EnemyTeleportState(const float minRange, const float maxRange);
	~EnemyTeleportState() override = default;

	/// <summary>
	/// ステートに入った時に呼ばれる処理
	/// </summary>
	/// <param name="enemy">処理対象の MobEnemy オブジェクトへのポインタ。ステートへ入る対象となる敵を指す</param>
	void Enter(MobEnemy* enemy) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

private:

	float minRange_ = 0.0f; // テレポートの最小距離
	float maxRange_ = 0.0f; // テレポートの最大距離
};

