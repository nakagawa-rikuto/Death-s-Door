#pragma once
/// ===Include=== ///
#include <application/Game/Entity/Enemy/BossEnemy/State/Base/BossState.h>
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

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
	/// ===フェーズ=== ///
	enum class Phase {
		Idle,       // 非アクティブ
		WindUp,     // 予備動作
		Strike,     // 攻撃
		HoldDown,   // 余韻
		Recovery,   // 回復
		Finished,   // 攻撃終了
	};
	Phase phase_ = Phase::Idle;

	/// ===現在の状態=== ///
	struct State {
		float phaseTimer = 0.0f;  // フェーズの経過時間
		Vector3 velocity{};       // 攻撃中の移動速度
		Quaternion rotation{};    // 攻撃中の回転
		Vector3 weaponPosition{}; // 攻撃中の武器位置オフセット
	};
	State state_{};

	// 攻撃開始時のBoss基底回転
	Quaternion startRotation{};

	// 攻撃終了フラグ
	bool isFinished_ = false;

private:

	/// <summary>
	/// 攻撃開始処理
	/// </summary>
	/// <param name="rotate">攻撃の回転</param>
	void StartAttack(const Quaternion& rotate);

	/// <summary>
	/// タイマーの更新
	/// </summary>
	void UpdateTimer();

	/// <summary>
	/// 攻撃の更新を実行します。
	/// </summary>
	void UpdateAttack();

	/// <summary>
	/// ワインドアップを更新します。
	/// </summary>
	void UpdateWindUp();

	/// <summary>
	/// ストライク情報を更新します。
	/// </summary>
	void UpdateStrike();

	/// <summary>
	/// ホールドダウンの状態を更新します。
	/// </summary>
	void UpdateHoldDown();

	/// <summary>
	/// リカバリー処理を更新します。
	/// </summary>
	void UpdateRecovery();
};

