#pragma once
/// ===Include=== ///
// Bullet
#include "BossEnemyBullet.h"
// AttackComponent
#include "application/Game/Entity/Enemy/BossEnemy/Component/Attack/LongAttack/BossAttackOrbitingOrbsComponent.h"
// C++
#include <memory>
#include <array>
#include <vector>

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
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

public:/// ===OrbitBullet=== ///

	/// <summary>
	/// OrbitBulletの生成
	/// </summary>
	/// <param name="positions"></param>
	/// <param name="lifeTime"></param>
	void SpawnOrbitingBullets(const std::array<Vector3, BossAttackOrbitingOrbsComponent::kOrbCount>& positions, float lifeTime);

	/// <summary>
	/// OrbitBulletの速度の設定
	/// </summary>
	/// <param name="velocities"></param>
	void SetOrbitingVelocities(const std::array<Vector3, BossAttackOrbitingOrbsComponent::kOrbCount>& velocities);

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

	// OrbitBulletの管理
	std::array<std::unique_ptr<BossEnemyBullet>, BossAttackOrbitingOrbsComponent::kOrbCount> orbitingBullets_{};

	// 放物線ショットの弾の管理
	std::vector<std::unique_ptr<BossEnemyBullet>> parabolicBullets_{};

private:

	/// <summary>
	/// OrbitBulletの更新処理
	/// </summary>
	void UpdateOrbitingBullets();

	/// <summary>
	/// ParabolicShotの更新処理
	/// </summary>
	void UpdateParabolicBullets();
};
	
