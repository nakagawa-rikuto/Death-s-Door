#pragma once
/// ===include=== ///
// BaseEnemy
#include "application/Game/Entity\Enemy/Base/BaseEnemy.h"
// State
#include "application/Game/Entity/Enemy/MobEnemy/State/Base/EnemyState.h"
// Component
#include "application/Game/Entity/Enemy/MobEnemy/Component/EnemyMoveComponent.h"
#include "application/Game/Entity/Enemy/MobEnemy/Component/EnemyHitReactionComponent.h"
// C++
#include <random>

/// ===前方宣言=== ///
namespace MiiEngine {
	class ParticleGroup;
}

///=====================================================/// 
/// MobEnemy
///=====================================================///
class MobEnemy : public BaseEnemy {
private: /// ===型定義=== ///

	/// ===攻撃情報=== ///
	struct AttackInfo {
		float timer;		// タイマー
		float range;		// 攻撃範囲(回転の情報から±)
		float distance;		// 攻撃可能距離
		float interval;		// 攻撃間隔
		int32_t power;		// 待機時間

		Vector3 direction;  // 攻撃方向
		Vector3 targetPos;  // プレイヤーの位置

		bool isAttack;		// 攻撃フラグ
	};

public:

	MobEnemy() = default;
	virtual ~MobEnemy();

	/// <summary>
	/// ゲームシーンを初期化
	/// </summary>
	/// <param name="translate">初期位置を表す3次元ベクトル。</param>
	virtual void InitGameScene(const Vector3& translate) override;

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// 描画を実行します
	/// </summary>
	/// <param name="mode">使用するブレンドモード。デフォルトは通常ブレンドモードです。</param>
	virtual void Draw(MiiEngine::BlendMode mode = MiiEngine::BlendMode::KBlendModeNormal) override;

	/// <summary>
	/// ImGui情報を表示
	/// </summary>
	virtual void Information() override;

	/// <summary>
	/// 攻撃処理の初期化
	/// </summary>
	virtual void StartAttack() = 0;

	/// <summary>
	/// 攻撃処理
	/// </summary>
	virtual void Attack() = 0;

public: /// ===衝突=== ///

	/// <summary>
	/// 衝突が発生したときに呼び出されます。
	/// </summary>
	/// <param name="collider">衝突したコライダーへのポインター。</param>
	virtual void OnCollision(MiiEngine::Collider* collider) override;

public: /// ===その他関数=== ///

	/// <summary>
	/// 敵の状態を変更します。
	/// </summary>
	/// <param name="nextState">次の敵の状態を表すユニークポインタ。</param>
	void ChangeState(std::unique_ptr<EnemyState> nextState);

	/// <summary>
	/// 攻撃可能かどうかを確認します。
	/// </summary>
	/// <returns>攻撃可能な場合は true、それ以外の場合は false。</returns>
	bool CheckAttackable();

public: /// ===Getter=== ///

	// Component
	EnemyMoveComponent& GetMovementComponent() const { return *moveComponent_; };
	EnemyHitReactionComponent& GetHitReactionComponent() const { return *hitReactionComponent_; };
	// AttackInfo
	AttackInfo GetAttackInfo()const { return attackInfo_; };
	// tentativeDeath
	bool GetTentativeDeath() const { return isTentativeDeath_; };

public: /// ===Setter=== ///
	
	// AttackDirection
	void SetAttackDirection(const Vector3& dir) { attackInfo_.direction = dir; };
	void SetTargetPos(const Vector3& pos) { attackInfo_.targetPos = pos; };

protected: /// ===変数の宣言=== ///

	// 攻撃情報
	AttackInfo attackInfo_;

	// 衝突フラグ
	bool isCollision_ = false;

private:
	/// ===State=== ///
	std::unique_ptr<EnemyState> currentState_;

	/// ===コンポーネント=== ///
	std::unique_ptr<EnemyMoveComponent> moveComponent_;
	std::unique_ptr<EnemyHitReactionComponent> hitReactionComponent_;

	/// ===Particle=== ///
	MiiEngine::ParticleGroup* hitParticle_ = nullptr;
	MiiEngine::ParticleGroup* deathParticle_ = nullptr;

	// 消えるまでの時間
	float disappearTimer_ = 2.0f;
private:
	/// <summary>
	/// パラメータを設定する処理
	/// </summary>
	void SettingParamita();

	/// <summary>
	/// タイマーを進める処理
	/// </summary>
	void advanceTimer();
};

