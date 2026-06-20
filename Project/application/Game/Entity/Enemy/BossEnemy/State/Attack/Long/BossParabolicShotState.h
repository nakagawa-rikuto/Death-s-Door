#pragma once
/// ===Include=== ///
#include <application/Game/Entity/Enemy/BossEnemy/State/Base/BossState.h>
// Math
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

/// ===前方宣言=== ///
class BossBulletManager;

///=====================================================/// 
/// BossParabolicShotState
///=====================================================///
class BossParabolicShotState : public BossState {
public:
	~BossParabolicShotState() override = default;

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

	// BulletManagerへの参照
	BossBulletManager* bulletManager_ = nullptr;

	/// ===フェーズ=== ///
	enum class Phase {
		Idle,       // 待機フェーズ
		LockOn,		// 狙いを定めるフェーズ
		Flying,     // 飛行フェーズ
		Finished    // 終了フェーズ
	};
	Phase phase_ = Phase::Idle;

	/// ===状態=== ///
	struct State {
		float lifeTimer = 0.0f;       // 攻撃の生存時間
		float lockOnTimer = 0.0f;     // LockOnの経過時間
		Vector3 bulletVelocity{};     // 弾のvelocity
		Vector3 bulletPosition{};     // 弾のワールド座標
		Quaternion rotate{};		  // LockOn中の顔の向き
	};
	State state_{};

	// 地面の情報
	float groundYPos_ = 0.0f;

	// 弾の初速
	float initialSpeed_ = 0.0f;

	// 着弾点
	Vector3 hitPosition_{};

	// 着弾したかのフラグ
	bool isHitGround_ = false;

private:

	/// <summary>
	/// 攻撃の開始処理
	/// </summary>
	/// <param name="groundPosY">地面のY座標</param>
	void StartAttack(float groundPosY);

	/// <summary>
	/// 攻撃の更新処理
	/// </summary>
	void UpdateAttack();

	/// <summary>
	/// ロックオンフェーズの更新処理
	/// </summary>
	void UpdateLockOn();

	/// <summary>
	/// 飛行フェーズの更新処理
	/// </summary>
	void UpdateFlying();

	/// <summary>
	/// 発射初速ベクトルを計算する。
	/// </summary>
	/// <param name="from">発射元の位置</param>
	/// <param name="to">ターゲットの位置</param>
	/// <returns>初速ベクトル</returns>
	Vector3 CalcInitialVelocity(const Vector3& from, const Vector3& to) const;
};

