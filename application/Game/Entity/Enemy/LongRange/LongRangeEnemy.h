#pragma once
/// ===Include=== ///
#include "application/Game/Entity/Enemy/Base/BaseEnemy.h"
#include "application/Game/Entity/Enemy/LongRange/LongRangeEnemeyBullet.h"
#include <list>

///=====================================================/// 
/// 遠距離攻撃Enemy
///=====================================================///
class LongRangeEnemy : public BaseEnemy {
public:
	LongRangeEnemy() = default;
	~LongRangeEnemy();

	// 初期化
	void Initialize()override;
	// 更新
	void Update()override;
	// 描画
	void Draw(BlendMode mode = BlendMode::KBlendModeNormal)override;
	// ImGui
	void UpdateImGui()override;
	// 攻撃処理
	void Attack()override;

public: /// ===衝突判定=== ///
	void OnCollision(Collider* collider) override;

public: /// ===Setter=== ///
	// Translate
	void SetTranslate(const Vector3& pos);
	void SetlastYaw();

private: /// ===変数=== ///

	/// ===Bullet情報=== ///
	struct BulletInfo {
		std::list<std::unique_ptr<LongRangeEnemeyBullet>> bullets_;
		float interval = 0.0f; // インターバル
		float reloadTime = 3.0f; // リロード時間
		bool isShot; // 発射フラグ
		bool isHit; // ヒット時のフラグ
	};
	BulletInfo bulletInfo_;

	// 前フレームのYawを保持
	float lastYaw_;
};
