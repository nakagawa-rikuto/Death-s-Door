#pragma once
/// ===Include=== ///
// AttackComponent
#include "CloseAttack/BossAttackDownwardSwingComponent.h"
#include "CloseAttack/BossAttackJumpSmashComponent.h"
#include "CloseAttack/BossAttackRotateComponent.h"
// C++
#include <memory>

///=====================================================/// 
/// BossEnemyの攻撃コンポーネントと条件を管理するクラス
///=====================================================///
class BossAttackManager {
public:
	/// ===設定パラメータの構造体=== ///
	struct Config {
		// 攻撃射程
		float rotateRange = 14.0f;
		float downswingRange = 20.0f;
		float jumpSmashMinRange = 30.0f;
		float jumpSmashMaxRange = 50.0f;

		// 個別クールダウン（秒）
		float rotateCooldown = 2.5f;
		float downswingCooldown = 4.0f;
		float jumpSmashCooldown = 8.0f;

		// 各攻撃コンポーネントのConfig
		BossAttackRotateComponent::RotateConfig rotateConfig{};
		BossAttackDownwardSwingComponent::DownwardSwingConfig downswingConfig{};
		BossAttackJumpSmashComponent::JumpSmashConfig jumpSmashConfig{};
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
	/// 毎フレーム呼び出す更新処理（タイマーの更新）
	/// </summary>
	void UpdateTimers(float deltaTime);

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information();

public: /// ===攻撃判定=== ///
	// 回転攻撃
	bool CanRotate(float distToPlayer) const { return distToPlayer <= config_.rotateRange && cooldowns_.rotate <= 0.0f;}
	// 振り下ろし攻撃
	bool CanDownswing(float distToPlayer) const { return distToPlayer <= config_.downswingRange && cooldowns_.downswing <= 0.0f;}
	// ジャンプ叩きつけ攻撃
	bool CanJumpSmash(float distToPlayer) const { return config_.jumpSmashMinRange <= distToPlayer && distToPlayer <= config_.jumpSmashMaxRange && cooldowns_.jumpSmash <= 0.0f;}

public: /// ===クールダウン設定=== ///

	void StartRotateCooldown() { cooldowns_.rotate = config_.rotateCooldown; }
	void StartDownswingCooldown() { cooldowns_.downswing = config_.downswingCooldown; }
	void StartJumpSmashCooldown() { cooldowns_.jumpSmash = config_.jumpSmashCooldown; }

public: /// ===Getter=== ///

	float GetRotateCooldown() const { return cooldowns_.rotate; }
	float GetDownswingCooldown() const { return cooldowns_.downswing; }
	float GetJumpSmashCooldown() const { return cooldowns_.jumpSmash; }

	const Config& GetConfig() const { return config_; }

	// 各コンポーネントへの参照（BossEnemyでImGui表示や各Stateでの使用）
	BossAttackRotateComponent& GetRotateComponent() { return *rotate_; }
	BossAttackDownwardSwingComponent& GetDownswingComponent() { return *downswing_; }
	BossAttackJumpSmashComponent& GetJumpSmashComponent() { return *jumpSmash_; }

#ifdef USE_IMGUI
public: /// ===Setter（ImGui用）=== ///
	void ApplyConfig(const Config& newConfig);
#endif // USE_IMGUI

private:

	/// ===設定・状態=== ///
	Config config_{};

	/// ===個別クールダウン=== ///
	struct Cooldowns {
		float rotate = 0.0f;
		float downswing = 0.0f;
		float jumpSmash = 0.0f;
	};
	Cooldowns cooldowns_{};

	/// ===攻撃コンポーネント=== ///
	std::unique_ptr<BossAttackRotateComponent> rotate_{};
	std::unique_ptr<BossAttackDownwardSwingComponent> downswing_{};
	std::unique_ptr<BossAttackJumpSmashComponent> jumpSmash_{};
};

