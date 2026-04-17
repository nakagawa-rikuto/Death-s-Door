#pragma once
/// ===Include=== ///
// BaseEnemy
#include "application/Game/Entity/Enemy/Base/BaseEnemy.h"
// Weapon
#include "Weapon/BossWeapon.h"
// State
#include "State/Base/BossState.h"
// Component
#include "Component/Move/BossMoveComponent.h"
#include "Component/Move/BossTeleportComponent.h"
#include "Component/Attack/BossAttackManager.h"
#include "Component/HitReaction/BossHitReactionComponent.h"
// C++
#include <vector>


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

public: /// ===Getter=== ///

	// Weaponの取得
	BossWeapon& GetWeapon() const { return *weapon_; }

	// Componentの取得
	BossMoveComponent& GetMoveComponent() const { return *moveComponent_; }
	BossTeleportComponent& GetTeleportComponent() const { return *teleportComponent_; }

	BossAttackRotateComponent& GetRotateComponent() const { return attackManager_->GetRotateComponent(); }
	BossAttackDownwardSwingComponent& GetDownswingComponent() const { return attackManager_->GetDownswingComponent(); }
	BossAttackJumpSmashComponent& GetJumpSmashComponent() const { return attackManager_->GetJumpSmashComponent(); }
	BossAttackOrbitingOrbsComponent& GetOrbitingOrbsComponent() const { return attackManager_->GetOrbitingOrbsComponent(); }
	BossAttackParabolicShotComponent& GetParabolicShotComponent() const { return attackManager_->GetParabolicShotComponent(); }
	BossAttackManager& GetAttackManager() const { return *attackManager_; }

	BossHitReactionComponent& GetHitReactionComponent() const { return *hitReactionComponent_; }

private:
	/// ===Weapon=== ///
	std::unique_ptr<BossWeapon> weapon_; // 武器

	/// ===State=== ///
	std::unique_ptr<BossState> currentState_; // 現在のState

	/// ===Component=== ///
	std::unique_ptr<BossMoveComponent> moveComponent_;	// 移動コンポーネント
	std::unique_ptr<BossTeleportComponent> teleportComponent_; // テレポートコンポーネント
	std::unique_ptr<BossAttackManager> attackManager_; // 攻撃マネージャー
	std::unique_ptr<BossHitReactionComponent> hitReactionComponent_; // 被ダメージリアクションコンポーネント

	/// ===Particle=== ///
	MiiEngine::ParticleGroup* hitParticle_ = nullptr;
	MiiEngine::ParticleGroup* deathParticle_ = nullptr;

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

