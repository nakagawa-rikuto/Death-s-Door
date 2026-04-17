#pragma once
/// ===Include=== ///
// AttackComponent
#include "CloseAttack/BossAttackDownwardSwingComponent.h"
#include "CloseAttack/BossAttackJumpSmashComponent.h"
#include "CloseAttack/BossAttackRotateComponent.h"
#include "LongAttack/BossAttackOrbitingOrbsComponent.h"
#include "LongAttack/BossAttackParabolicShotComponent.h"
// Bullet
#include <application/Game/Entity/Enemy/BossEnemy/Bullet/BossEnemyBullet.h>
// C++
#include <memory>
#include <array>

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
		float orbitingOrbsRange = 40.0f;
		float parabolicShotRange = 35.0f;

		// 個別クールダウン（秒）
		float rotateCooldown = 2.5f;
		float downswingCooldown = 4.0f;
		float jumpSmashCooldown = 8.0f;
		float orbitingOrbsCooldown = 6.0f;
		float parabolicShotCooldown = 5.0f;

		// 各攻撃コンポーネントのConfig
		BossAttackRotateComponent::RotateConfig rotateConfig{};
		BossAttackDownwardSwingComponent::DownwardSwingConfig downswingConfig{};
		BossAttackJumpSmashComponent::JumpSmashConfig jumpSmashConfig{};
		BossAttackOrbitingOrbsComponent::OrbitConfig orbitingOrbsConfig{};
		BossAttackParabolicShotComponent::ParabolicConfig parabolicShotConfig{};
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
	/// 更新処理
	/// </summary>
	void Update(const Vector3& bossPosition, float deltaTime);

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information();

public: /// ===攻撃判定=== ///
	// 回転攻撃
	bool CanRotate(float distToPlayer) const { return distToPlayer <= config_.rotateRange && cooldowns_.rotate <= 0.0f; }
	// 振り下ろし攻撃
	bool CanDownswing(float distToPlayer) const { return distToPlayer <= config_.downswingRange && cooldowns_.downswing <= 0.0f; }
	// ジャンプ叩きつけ攻撃
	bool CanJumpSmash(float distToPlayer) const { return config_.jumpSmashMinRange <= distToPlayer && distToPlayer <= config_.jumpSmashMaxRange && cooldowns_.jumpSmash <= 0.0f; }
	// 周りを回るオーブ攻撃
	bool CanOrbitIngOrbs(float distToPlayer) const { return distToPlayer <= config_.orbitingOrbsRange && cooldowns_.orbitingOrbs <= 0.0f; }
	// 放物線ショット攻撃
	bool CanParabolicShot(float distToPlayer) const { return distToPlayer <= config_.parabolicShotRange && cooldowns_.parabolicShot <= 0.0f; }

public: /// ===攻撃の開始=== ///
	// OrbitingOrbs
	void StartOrbitingOrbs(const Vector3& bossPos);
	// ParabolicShot
	void StartParabolicShot(const Vector3& bossPos, const Vector3& playerPos, float groundY);

public: /// ===クールダウン設定=== ///

	void StartRotateCooldown() { cooldowns_.rotate = config_.rotateCooldown; }
	void StartDownswingCooldown() { cooldowns_.downswing = config_.downswingCooldown; }
	void StartJumpSmashCooldown() { cooldowns_.jumpSmash = config_.jumpSmashCooldown; }
	void StartOrbitingOrbsCooldown() { cooldowns_.orbitingOrbs = config_.orbitingOrbsCooldown; }
	void StartParabolicShotCooldown() { cooldowns_.parabolicShot = config_.parabolicShotCooldown; }

public: /// ===Getter=== ///

	float GetRotateCooldown() const { return cooldowns_.rotate; }
	float GetDownswingCooldown() const { return cooldowns_.downswing; }
	float GetJumpSmashCooldown() const { return cooldowns_.jumpSmash; }
	float GetOrbitingOrbsCooldown() const { return cooldowns_.orbitingOrbs; }
	float GetParabolicShotCooldown() const { return cooldowns_.parabolicShot; }

	const Config& GetConfig() const { return config_; }

	// 各コンポーネントへの参照（BossEnemyでImGui表示や各Stateでの使用）
	BossAttackRotateComponent& GetRotateComponent() { return *rotate_; }
	BossAttackDownwardSwingComponent& GetDownswingComponent() { return *downswing_; }
	BossAttackJumpSmashComponent& GetJumpSmashComponent() { return *jumpSmash_; }
	BossAttackOrbitingOrbsComponent& GetOrbitingOrbsComponent() { return *orbitingOrbs_; }
	BossAttackParabolicShotComponent& GetParabolicShotComponent() { return *parabolicShot_; }

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
		float orbitingOrbs = 0.0f;
		float parabolicShot = 0.0f;
	};
	Cooldowns cooldowns_{};

	/// ===攻撃コンポーネント=== ///
	std::unique_ptr<BossAttackRotateComponent> rotate_{};
	std::unique_ptr<BossAttackDownwardSwingComponent> downswing_{};
	std::unique_ptr<BossAttackJumpSmashComponent> jumpSmash_{};
	std::unique_ptr<BossAttackOrbitingOrbsComponent> orbitingOrbs_{};
	std::unique_ptr<BossAttackParabolicShotComponent> parabolicShot_{};

	// OrbeBulletの管理
	std::array<std::unique_ptr<BossEnemyBullet>, BossAttackOrbitingOrbsComponent::kOrbCount> orbitingBullets_{};

	// 放物線ショットの弾の管理
	std::unique_ptr<BossEnemyBullet> parabolicShotBullet_{};

private:
	/// <summary>
	/// 毎フレーム呼び出す更新処理（タイマーの更新）
	/// </summary>
	void UpdateTimers(float deltaTime);
};

