#pragma once
/// ===Include=== ///
// Bullet
#include "BossEnemyBullet.h"
// Calculator
#include "Calculator/BossOrbitingOrbsCalculator.h"
// C++
#include <memory>
#include <array>
#include <vector>

/// ===前方宣言=== ///
namespace BossComponent {
	struct OrbitingAttackComponent;
}

///=====================================================/// 
/// BossEnemyの弾の管理クラス
///=====================================================///
class BossBulletManager {
public:

	BossBulletManager() = default;
	~BossBulletManager();
	BossBulletManager(const BossBulletManager&) = delete;
	BossBulletManager& operator=(const BossBulletManager&) = delete;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const BossComponent::OrbitingAttackComponent& component);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(float deltaTime);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

public:/// ===OrbitBullet=== ///

	/// <summary>
    /// OrbitBulletの生成
    /// </summary>
	void SpawnOrbitingBullets();

	/// <summary>
	/// OrbitBulletの停止
	/// </summary>
	void StopOrbitingBullets();

public: /// ===ParabolicShot=== ///

	/// <summary>
	/// ParabolicShotの生成
	/// </summary>
	/// <param name="position"></param>
	/// <param name="lifeTime"></param>
	void SpawnParabolicBullets(const Vector3& position,float lifeTime);

	/// <summary>
	/// ParabolicShotの速度の設定
	/// </summary>
	/// <param name="velocity"></param>
	void SetParabolicVelocity(const Vector3& velocity);

	/// <summary>
	/// ParabolicShotの弾を削除
	/// </summary>
	void KillLatestParabolicBullets();

public: /// ===Getter=== ///
	// ParabolicShotの弾の位置を取得
	Vector3 GetParabolicBulletPosition() const;

private:

	/// ===OrbitBullet=== ///
	struct OrbitBulletInfo {
		int bulletCount = 0; // OrbitBulletの数
		float lifeTime = 0.0f; // OrbitBulletの生存時間
		float orbitSpeed = 0.0f; // OrbitBulletの公転速度
	};
	std::vector<std::unique_ptr<BossEnemyBullet>> orbitingBullets_{};
	OrbitBulletInfo orbitInfo_{};
	// OrbitBulletのアクティブ状態
	bool isOrbitingActive_ = false; 
	// OrbitBulletの位置と向きを計算するクラス
	std::unique_ptr<BossOrbitingOrbsCalculator> orbitCalculator_; 


	/// ===ParabolicBullet=== ///
	std::vector<std::unique_ptr<BossEnemyBullet>> parabolicBullets_{};

private:

	/// <summary>
	/// OrbitBulletの更新処理
	/// </summary>
	void UpdateOrbitingBullets(float deltaTime);

	/// <summary>
	/// ParabolicShotの更新処理
	/// </summary>
	void UpdateParabolicBullets(float deltaTime);
};
	
