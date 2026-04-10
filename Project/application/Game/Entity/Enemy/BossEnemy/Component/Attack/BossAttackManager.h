#pragma once
/// ===Include=== ///
// AttackComponent
#include "CloseAttack/BossAttackDownwardSwingComponent.h"
#include "CloseAttack/BossAttackJumpSmashComponent.h"
#include "CloseAttack/BossAttackRotateComponent.h"
// C++
#include <memory>

///=====================================================/// 
/// BossEnemyの攻撃を管理するクラス
/// 攻撃の選択・クールダウン・実行を一括管理する
///=====================================================///
class BossAttackManager {
private:
	/// ===攻撃の種類=== ///
	enum class AttackType {
		None,
		Thrust,        // 突き
		DownwardSwing, // 振り下ろし
		JumpSmash,     // ジャンプ叩きつけ
	};

public:
	/// ===設定パラメータの構造体=== ///
	struct Config {
		// 攻撃射程
		float thrustRange = 4.0f;
		float downswingRange = 6.0f;
		float jumpSmashRange = 14.0f;

		// 個別クールダウン（秒）
		float thrustCooldown = 3.0f;
		float downswingCooldown = 4.0f;
		float jumpSmashCooldown = 6.0f;

		// 各攻撃コンポーネントのConfig
		BossAttackRotateComponent::RotateConfig thrustConfig{};
		BossAttackDownwardSwingComponent::DownwardSwingConfig downswingConfig{};
		BossAttackJumpSmashComponent::JumpSmashConfig jumpSmashConfig{};
	};

	/// ===更新用コンテキスト=== ///
	struct UpdateContext {
		Quaternion bossRotation{};  // ボスの現在回転
		Vector3    bossPosition{};  // ボスの現在位置（JumpSmash用）
		float      deltaTime = 0.0f;
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		AttackType currentAttack = AttackType::None; // 現在実行中の攻撃
		Quaternion modelRotation{};                  // モデル回転（各Componentの結果）
		Vector3    weaponLocalOffset{};              // 武器オフセット
		Vector3    modelPositionDelta{};             // 踏み込みなどの位置オフセット
		bool       isAttacking = false;             // 攻撃実行中か
		bool       justFinished = false;             // このフレームで攻撃が完了したか
	};

public:

	BossAttackManager() = default;
	~BossAttackManager() = default;

	BossAttackManager(const BossAttackManager&) = delete;
	BossAttackManager& operator=(const BossAttackManager&) = delete;

	/// <summary>
	/// 初期化（生成時に一度だけ呼ぶ）
	/// </summary>
	void Initialize(const Config& config = Config{});

	/// <summary>
	/// 毎フレーム呼び出す更新処理。
	/// クールダウンの減算と、実行中攻撃の更新を行う。
	/// </summary>
	UpdateResult Update(const UpdateContext& context);

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information();

	/// <summary>
	/// 距離に応じた攻撃を選択して開始する。
	/// クールダウン済みかつ射程内の攻撃から選ぶ。
	/// </summary>
	/// <param name="distToPlayer">プレイヤーとの距離</param>
	/// <param name="bossPosition">ボスの現在位置（JumpSmash用）</param>
	/// <param name="playerPosition">プレイヤーの位置（JumpSmash用）</param>
	/// <param name="bossRotation">ボスの現在回転</param>
	/// <returns>選択・開始した攻撃の種類。何も選べなければ None</returns>
	AttackType SelectAndStart(
		float distToPlayer,
		const Vector3& bossPosition,
		const Vector3& playerPosition,
		const Quaternion& bossRotation
	);

	/// <summary>
	/// 現在の距離で使用可能な攻撃が1つでもあるか返す。
	/// AttackBossState / MoveBossState の遷移判定で使用する。
	/// </summary>
	bool IsAnyAttackAvailable(float distToPlayer) const;

	/// <summary>
	/// 実行中の攻撃を強制終了してリセットする。
	/// State の強制遷移時などに使用する。
	/// </summary>
	void ForceReset();

public: /// ===Getter=== ///
	AttackType GetCurrentAttack() const { return currentAttack_; }
	bool IsAttacking() const { return currentAttack_ != AttackType::None; }

	float GetThrustCooldown()    const { return cooldowns_.thrust; }
	float GetDownswingCooldown() const { return cooldowns_.downswing; }
	float GetJumpSmashCooldown() const { return cooldowns_.jumpSmash; }

	const Config& GetConfig() const { return config_; }

	// 各コンポーネントへの参照（BossEnemyでImGui表示に使用）
	BossAttackRotateComponent& GetThrustComponent() { return *thrust_; }
	BossAttackDownwardSwingComponent& GetDownswingComponent() { return *downswing_; }
	BossAttackJumpSmashComponent& GetJumpSmashComponent() { return *jumpSmash_; }

#ifdef USE_IMGUI
public: /// ===Setter（ImGui用）=== ///
	void ApplyConfig(const Config& newConfig);
#endif // USE_IMGUI

private:

	/// ===設定・状態=== ///
	Config config_{};
	AttackType currentAttack_ = AttackType::None;

	/// ===個別クールダウン=== ///
	struct Cooldowns {
		float thrust = 0.0f;
		float downswing = 0.0f;
		float jumpSmash = 0.0f;
	};
	Cooldowns cooldowns_{};

	/// ===攻撃コンポーネント=== ///
	std::unique_ptr<BossAttackRotateComponent> thrust_{};
	std::unique_ptr<BossAttackDownwardSwingComponent> downswing_{};
	std::unique_ptr<BossAttackJumpSmashComponent> jumpSmash_{};

private:

	/// <summary>
	/// 指定した攻撃のクールダウンをConfigの値でセットする。
	/// </summary>
	void SetCooldown(AttackType type);

	/// <summary>
	/// 全クールダウンタイマーをdeltaTimeで減算する。
	/// </summary>
	void UpdateCooldowns(float deltaTime);

	/// <summary>
	/// 実行中コンポーネントを更新し結果を返す。
	/// </summary>
	UpdateResult UpdateCurrentAttack(const UpdateContext& context);
};

