#pragma once
/// ===Include=== ///
#include <application/Game/Entity/Enemy/BossEnemy/State/Base/BossState.h>
// Math
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

///=====================================================/// 
/// BossRotateAttackState
/// 突き攻撃の状態を表すクラス
///=====================================================///
class BossRotateAttackState : public BossState {
public:
	~BossRotateAttackState() override = default;

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
		Idle,       // 非アクティブ（待機）
		WindUp,     // 予備動作（体をひねってタメる）
		Strike,     // 攻撃（切り返して武器を突き出す）
		Recovery,   // 回復（-strikeAngle → 0° へゆっくり戻る）
		Finished,   // 攻撃終了
	};
	Phase phase_ = Phase::Idle; // 現在のフェーズ

	/// ===状態=== ///
	struct State {
		float phaseTimer = 0.0f;   // 現フェーズの経過時間
		Quaternion rotate{};       // 攻撃中の回転
		Vector3 weaponPosition{};    // 攻撃中の武器位置オフセット
	};
	State state_{}; // 状態

	// 攻撃フラグ
	bool isAttacking_ = false;

	// 攻撃終了フラグ
	bool isFinished_ = false; 

private:

	/// <summary>
	/// 攻撃開始処理
	/// </summary>
	void StartAttack();

	/// <summary>
	/// タイマーの更新
	/// </summary>
	void UpdateTimer();

	/// <summary>
	/// 攻撃の更新を実行します。
	/// </summary>
	void UpdateAttack();

	/// <summary>
	/// 予備動作の更新を実行します。	
	/// </summary>
	void UpdateWindUp();

	/// <summary>
	/// 攻撃の更新を実行します。
	/// </summary>
	void UpdateStrike();

	/// <summary>
	/// 回復の更新を実行します。
	/// </summary>
	void UpdateRecovery();
};

