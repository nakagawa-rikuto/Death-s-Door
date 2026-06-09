#pragma once
/// ===Include=== ///
#include <application/Game/Entity/Enemy/BossEnemy/State/Base/BossState.h>
// Math
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
// C++
#include <array>

/// ===前方宣言=== ///
class BossBulletManager;

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
};

