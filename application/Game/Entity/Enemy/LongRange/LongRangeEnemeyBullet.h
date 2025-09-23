#pragma once
/// ===Inclde=== ///
#include "Engine/Collider/SphereCollider.h"

///=====================================================/// 
/// 遠距離攻撃Enemyの弾
///=====================================================///
class LongRangeEnemeyBullet : public SphereCollider{
public:

	LongRangeEnemeyBullet() = default;
	~LongRangeEnemeyBullet();

	// 初期化
	void Initialize()override;
	// 更新
	void Update()override;
	// 描画
	void Draw(BlendMode mode = BlendMode::KBlendModeNormal)override;
	// 生成
	void Create(const Vector3& pos, const Vector3& vel);
	// 情報
	void Information() override {};

public: /// ===衝突判定=== ///
	void OnCollision(Collider* collider) override;

public: /// ===Getter=== ///
	bool GetIsAlive();

private: /// ===変数=== ///

	/// ===基本情報=== ///
	struct BaseInfo {
		Vector3 direction = { 0.0f, 0.0f, 0.0f }; // 方向
		Vector3 velocity = { 0.0f, 0.0f, 0.0f }; // ベクトル
	};
	BaseInfo info_;

	// 速度
	float speed_ = 1.5f;
	// 生存タイマー
	float lifeTimer_ = 0.0f;
	// 生存時間
	float lifeTime_ = 3.0f;
	// 生存フラグ
	bool isAlive_ = true;

private:

	// 移動処理
	void Move();
	// タイマー
	void PromoteTimer();
};

