#pragma once
/// ===Include=== ///
// BaseEnemy
#include "application/Game/Entity/Enemy/Base/BaseEnemy.h"
// Weapon
#include "Weapon/BossWeapon.h"
// State
#include "State/Base/BossState.h"
// Component
#include "Component/BossComponent.h"
// BulletManager
#include "Bullet/BossBulletManager.h"

///=====================================================/// 
/// BossEnemy
///=====================================================///
class BossEnemy : public BaseEnemy {
public:

	BossEnemy() = default;
	~BossEnemy();

	/// <summary>
	/// ゲームシーンで呼び出す初期化処理の純粋仮想関数
	/// </summary>
	/// <param name="translate">シーンの位置を指定する平行移動ベクトル。const参照で渡される。</param>
	virtual void InitGameScene(const Vector3& translate);

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode">描画に使用するブレンドモード。既定値は BlendMode::KBlendModeNormal。</param>
	virtual void Draw(MiiEngine::BlendMode mode = MiiEngine::BlendMode::KBlendModeNormal)override;

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	virtual void Information()override;

public: /// ===衝突判定=== ///

	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="collider">衝突した相手を表す Collider へのポインター。</param>
	void OnCollision(Collider* collider) override;

public: /// ===その他関数=== ///

	/// <summary>
	/// 敵の状態を変更します。
	/// </summary>
	/// <param name="nextState">次の敵の状態を表すユニークポインタ。</param>
	void ChangeState(std::unique_ptr<BossState> nextState);

	/// ===攻撃判定=== ///
	bool CanRotateAttack(float distToPlayer) const {return distToPlayer <= parameters_.attackRange.rotateAttack && cooldownTimer_.rotateAttack <= 0.0f; }
	bool CanDownSwingAttack(float distToPlayer) const {return distToPlayer <= parameters_.attackRange.downwardSwingAttack && cooldownTimer_.downwardSwingAttack <= 0.0f; }
	bool CanJumpSmashAttack(float distToPlayer) const {return parameters_.attackRange.jumpSmashMin <= distToPlayer && distToPlayer <= parameters_.attackRange.jumpSmashMax && cooldownTimer_.jumpSmashAttack <= 0.0f; }
	bool CanOrbitingOrbs(float distToPlayer) const {return distToPlayer <= parameters_.attackRange.orbitingOrbs && cooldownTimer_.orbitingOrbs <= 0.0f; }
	bool CanParabolicShot(float distToPlayer) const {return distToPlayer <= parameters_.attackRange.parabolicShot && cooldownTimer_.parabolicShot <= 0.0f; }

public: /// ===Getter=== ///

	// Weaponの取得
	BossWeapon& GetWeapon() const { return *weapon_; }
	// BulletManagerの取得
	BossBulletManager& GetBulletManager() const { return *bulletManager_; }
	// Componentのパラメータの取得
	const BossComponent::Parameters& GetComponentParameters() const { return parameters_; }

public: /// ===Setter=== ///
	// クールダウンの設定
	void SetRotateAttackCooldown() { cooldownTimer_.rotateAttack = parameters_.attackCooldown.rotateAttack; }
	void SetDownwardSwingAttackCooldown() { cooldownTimer_.downwardSwingAttack = parameters_.attackCooldown.downwardSwingAttack; }
	void SetJumpSmashAttackCooldown() { cooldownTimer_.jumpSmashAttack = parameters_.attackCooldown.jumpSmashAttack; }
	void SetOrbitingOrbsCooldown() { cooldownTimer_.orbitingOrbs = parameters_.attackCooldown.orbitingOrbs; }
	void SetParabolicShotCooldown() { cooldownTimer_.parabolicShot = parameters_.attackCooldown.parabolicShot; }

private:
	/// ===Weapon=== ///
	std::unique_ptr<BossWeapon> weapon_; // 武器

	/// ===State=== ///
	std::unique_ptr<BossState> currentState_; // 現在のState

	/// ===Component=== ///
	BossComponent::Parameters parameters_; // パラメータ

	/// ===BulletManager=== ///
	std::unique_ptr<BossBulletManager> bulletManager_;

	/// ===Particle=== ///
	MiiEngine::ParticleGroup* hitParticle_ = nullptr;
	MiiEngine::ParticleGroup* deathParticle_ = nullptr;

	/// ===クールダウン=== ///
	BossComponent::AttackCooldown cooldownTimer_;

private:

	/// <summary>
	/// コンポーネントのパラメータの設定
	/// </summary>
	void SetComponentConfig();

	/// <summary>
	/// タイマーを進める
	/// </summary>
	void advanceTimer();

	/// <summary>
	/// 死亡時の処理
	/// </summary>
	void DeathUpdate();
};

