#pragma once
/// ===Include=== ///
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <vector>

///=====================================================/// 
/// ParabolicShot
/// ターゲットに向かって放物線軌道で弾を発射する攻撃コンポーネント
///=====================================================///
class BossAttackParabolicShotComponent {
private:

	/// ===フェーズ定義=== ///
	enum class ParabolicPhase {
		Idle,       // 待機フェーズ
		Flying,     // 飛行フェーズ
		Finished    // 終了フェーズ
	};

	/// ===状態=== ///
	struct ParabolicState {
		Vector3 velocity{};		// 現在の速度ベクトル
		float lifeTimer = 0.0f; // 生存時間タイマー
	};

public:

	/// ===設定パラメータ=== ///
	struct ParabolicConfig {
		// --- 発射パラメータ ---
		float launchAngleDeg = 45.0f;  // 仰角（度）。大きいほど高い弧を描く
		float gravity = 9.8f;   // 重力加速度（Units/秒²）
		float lifetime = 5.0f;   // 弾の最大生存時間（秒）

		// --- 着弾判定 ---
		// 弾がターゲットの高さ（Y座標）を下回ったら着弾とみなす
		bool  enableGroundHit = true;   // 地面（Y<=groundY）で Finished にする
		float groundY = 0.0f;   // 地面のY座標

		// --- 水平速度の上限 ---
		float maxHorizontalSpeed = 20.0f; // 水平初速の上限（Units/秒）
	};

	/// ===更新用コンテキスト=== ///
	struct UpdateContext {
		Vector3 bulletPosition{}; // 現在の弾のワールド座標（BossEnemyBullet から取得）
		float deltaTime = 0.0f;
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		Vector3 velocity{};      // 正規化された移動方向 → SetDirection() へ渡す
		bool isFlying = false;
		bool isFinished = false;
	};

public:

	BossAttackParabolicShotComponent() = default;
	~BossAttackParabolicShotComponent() = default;

	BossAttackParabolicShotComponent(const BossAttackParabolicShotComponent&) = delete;
	BossAttackParabolicShotComponent& operator=(const BossAttackParabolicShotComponent&) = delete;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const ParabolicConfig& config = ParabolicConfig{});

	/// <summary>
	/// 更新処理
	/// </summary>
	UpdateResult Update(const UpdateContext& context);

	/// <summary>
	/// リセット処理
	/// </summary>
	void Reset();

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information();

	/// <summary>
	/// 攻撃の開始処理
	/// </summary>
	/// <param name="bossPosition">弾の発射位置</param>
	/// <param name="targetPosition">着弾点のワールド座標</param>
	/// <param name="groundPosY">地面のY座標</param>
	void StartAttack(const Vector3& bossPosition, const Vector3& targetPosition, float groundPosY);

public: /// ===Getter=== ///
	bool IsActive() const { return phase_ != ParabolicPhase::Idle; }
	bool isFinished() const { return phase_ == ParabolicPhase::Finished; }
	const ParabolicState& GetState() const { return state_; }

private:

	ParabolicConfig config_; // 攻撃の設定パラメータ
	ParabolicState state_;   // 攻撃の状態
	ParabolicPhase phase_ = ParabolicPhase::Idle; // 攻撃のフェーズ
	float initialSpeed_ = 0.0f; // 発射時の初速（Units/秒）

private:

	/// <summary>
	/// 発射初速ベクトルを計算する。
	/// </summary>
	/// <param name="from">`	</param>
	/// <param name="to"></param>
	/// <returns></returns>
	Vector3 CalcInitialVelocity(const Vector3& from, const Vector3& to) const;
};

