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

private:
	/// ===フェーズ=== ///
	enum class Phase {
		Anticipation, // 予備動作
		StartAttack,  // 攻撃開始
	};
	Phase currentPhase_ = Phase::Anticipation; // 現在のフェーズ

	/// ===予備動作情報=== ///
	struct AnticipationInfo {
		float time = 1.0f; // 予備動作の時間
		float maxScale = 1.2f; // 最大スケール
		float minScale = 0.8f; // 最小スケール
		float returnSpeed = 5.0f; // スケールを元に戻す速度
	};
	AnticipationInfo anticipationInfo_;

	/// ===情報=== ///
	struct State {
		float anticipationTimer = 0.0f; // 予備動作のタイマー
		Vector3 scale_ = { 1.0f, 1.0f, 1.0f }; // スケール
	};
	State state_;
};

