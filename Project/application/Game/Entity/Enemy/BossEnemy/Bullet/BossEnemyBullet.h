#pragma once
/// ===Include=== ///
#include <Engine/Collider/SphereCollider.h>

/// ===前方宣言=== ///
namespace MiiEngine {
	class ParticleGroup;
}

///=====================================================/// 
/// BossEnemyBullet
/// ボスの弾
///=====================================================///
class BossEnemyBullet : public MiiEngine::SphereCollider {
public:

	BossEnemyBullet() = default;
	~BossEnemyBullet();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode">ブレンドモード</param>
	void Draw(MiiEngine::BlendMode mode)override;

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information()override;

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="translate">生成位置</param>
	/// <param name="lifeTime">生存時間</param>
	void Create(const Vector3& translate, float lifeTime);

public: /// ===衝突判定=== ///

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="collider">コライダー</param>
	void OnCollision(Collider* collider) override;

public: /// ===Getter=== ///
	// 生存フラグの取得
	bool GetIsAlive() const { return info_.isAlive; };

public: /// ===Setter=== ///
	// 方向ベクトルの設定
	void SetVelocity(const Vector3& velocity) { info_.velocity = velocity; };

private: /// ===変数=== ///

	/// ===基本情報=== ///
	struct BaseInfo {
		Vector3 velocity = { 0.0f, 0.0f, 0.0f }; // ベクトル

		float lifeTime = 3.0f; // 生存時間
		bool isAlive = true; // 生存フラグ
	};
	BaseInfo info_;

	// ParticleGroup
	MiiEngine::ParticleGroup* bulletParticle_ = nullptr;

	// 生存タイマー
	float lifeTimer_ = 0.0f;

private:

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// タイマーの更新処理
	/// </summary>
	void PromoteTimer();
};

