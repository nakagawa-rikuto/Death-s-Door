#pragma once
/// ===Include=== ///
#include <application/Game/Entity/Enemy/BossEnemy/State/Base/BossState.h>

namespace MiiEngine {
	class ParticleGroup;
}

///=====================================================/// 
/// TeleportState
///=====================================================///
class BossTeleportState : public BossState {
public:
	BossTeleportState(const float minRange, const float maxRange);
	~BossTeleportState() override = default;

	/// <summary>
	/// ステートに入った時に呼ばれる処理
	/// </summary>
	/// <param name="enemy">処理対象の MobEnemy オブジェクトへのポインタ。ステートへ入る対象となる敵を指す</param>
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

	float minRange_ = 0.0f; // テレポートの最小距離
	float maxRange_ = 0.0f; // テレポートの最大距離

	MiiEngine::ParticleGroup* teleportParticle_ = nullptr; // テレポートエフェクトのパーティクル

	int teleportCount_ = 0; // テレポート回数
	const int kMaxTeleportCount = 3; // 最大テレポート回数
};

