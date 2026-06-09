#pragma once
/// ===Include=== ///
#include <application/Game/Entity/Enemy/BossEnemy/State/Base/BossState.h>
// C++
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <random>

/// ===前方宣言=== ///
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

	/// ===フェーズ=== ///
	enum class Phase {
		None,     // 演出なし
		SpinOut,  // その場でEaseIn回転しながら透明化（移動なし）
		Warp,     // 完全透明のまま NextPosition へ座標をワープ（1フレーム）
		SpinIn,   // NextPosition でEaseOut回転しながら不透明化（移動なし）
	};
	Phase phase_ = Phase::None;

	/// ===状態=== ///
	struct State {
		float phaseTimer = 0.0f;	 // フェーズ内の経過時間
		float colorAlpha = 1.0f;	 // 透明度
		int teleportCount = 0;		 // テレポート回数
		Vector3 position{};			 // 現在の座標
		Quaternion rotate{};         // スピン込みの現在の回転
		Vector3 velocity{};			 // 移動ベクトル
	};
	State state_{};

	// 開始時の回転
	Quaternion startRotation_{};

	// ワープ時の座標
	Vector3 startPosition_{}; // ワープ元座標
	Vector3 nextPosition_{};  // ワープ先座標

	/// ===ランダムエンジン=== ///
	std::mt19937 randomEngine_;

	// テレポートの距離範囲
	float minRange_ = 0.0f; // 最小距離
	float maxRange_ = 0.0f; // 最大距離

	// テレポートエフェクトのパーティクル
	MiiEngine::ParticleGroup* teleportParticle_ = nullptr; 

	// 最大テレポート回数
	const int kMaxTeleportCount_ = 1;

	// 終了フラグ
	bool isFinished_ = false;

private:

	/// <summary>
	/// テレポート開始処理
	/// </summary>
	void StartTeleport();

	/// <summary>
	/// 次の位置を設定
	/// </summary>
	/// <param name="minRange">最小範囲。</param>
	/// <param name="maxRange">最大範囲。</param>
	/// <param name="currentPosition">現在の位置。</param>
	/// <returns>次の位置を表すVector3。</returns>
	Vector3 SetNextPosition(float minRange, float maxRange, const Vector3& currentPosition);

	/// <summary>
	/// テレポート更新処理
	/// </summary>
	void UpdateTeleport();

	/// <summary>
	/// SpinOutフェーズの更新処理
	/// </summary>
	void UpdateSpinOut();

	/// <summary>
	/// Warpフェーズの更新処理
	/// </summary>
	void UpdateWarp();

	/// <summary>
	/// SpinInフェーズの更新処理
	/// </summary>
	void UpdateSpinIn();
};

