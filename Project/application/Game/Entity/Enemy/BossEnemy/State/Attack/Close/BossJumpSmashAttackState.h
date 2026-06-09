#pragma once
/// ===Include=== ///
#include <application/Game/Entity/Enemy/BossEnemy/State/Base/BossState.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

///=====================================================/// 
/// BossJumpSmashAttackState
/// ジャンプして叩きつける攻撃の状態を表すクラス
///=====================================================///
class BossJumpSmashAttackState : public BossState {
public:
	~BossJumpSmashAttackState() override = default;

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
		Idle,        // 非アクティブ（待機）
		LeapWindUp,  // 跳躍前の溜め（屈んで重心を落とす）
		Leap,        // 放物線飛行
		Strike,      // 着地叩きつけ
		HoldDown,    // 叩きつけ余韻（深い前傾を一瞬維持）
		Recovery,    // 正位置へ戻る
		Finished,    // 攻撃終了
	};
	Phase phase_ = Phase::Idle;

	/// ===状態=== ///
	struct State {
		float phaseTimer = 0.0f;    // 現フェーズの経過時間
		float distance = 0.0f;		// 攻撃開始時のBossとPlayerの距離
		Vector3 velocity{};       // 攻撃中の移動速度
		Vector3 position{};       // 攻撃中のワールド座標位置
		Quaternion rotation{};    // 攻撃中の回転
		Vector3 weaponPosition{}; // 攻撃中の武器位置オフセット
	};
	State state_{};

	// 距離
	float distance_ = 0.0f;

	// 位置
	Vector3 startPosition_{};    // 跳躍開始位置（ワールド座標）
	Vector3 targetPosition_{};   // 着地目標位置（ワールド座標）

	// 回転
	Quaternion startRotation_{};  // 攻撃開始時の基底回転

	// 攻撃終了フラグ
	bool isFinished_ = false;

private:

	/// <summary>
	/// 攻撃開始処理
	/// </summary>
	/// <param name="distance">攻撃開始時のBossとPlayerの距離。攻撃の強さや飛距離に影響します。</param>
	void StartAttack(float distance);

	/// <summary>
	/// 攻撃状態を更新します。
	/// </summary>
	void UpdateAttack();

	/// <summary>
	/// リープワインドアップを更新します。
	/// </summary>
	void UpdateLeapWindUp();

	/// <summary>
	/// 放物線上の飛行を更新します。
	/// </summary>
	void UpdateLeap();

	/// <summary>
	/// ストライク情報を更新します。
	/// </summary>
	void UpdateStrike();

	/// <summary>
	/// ホールドダウン状態を更新します。
	/// </summary>
	void UpdateHoldDown();

	/// <summary>
	/// リカバリー処理を更新します。
	/// </summary>
	void UpdateRecovery();

	/// <summary>
	/// 放物線の高さを計算する。
	/// </summary>
	float CalcParabolaHeight(float t) const;
};

