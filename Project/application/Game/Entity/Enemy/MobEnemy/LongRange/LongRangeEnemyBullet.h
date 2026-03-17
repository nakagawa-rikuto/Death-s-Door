#pragma once
/// ===Include=== ///
#include "Engine/Collider/SphereCollider.h"

/// ===前方宣言=== ///
namespace MiiEngine {
	class ParticleGroup;
}
class Player;

///=====================================================/// 
/// 遠距離攻撃Enemyの弾
///=====================================================///
class LongRangeEnemyBullet : public MiiEngine::SphereCollider{
public:

	LongRangeEnemyBullet() = default;
	~LongRangeEnemyBullet();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode">描画時に使用するブレンドモード。省略時は BlendMode::KBlendModeNormal が使われます。</param>
	void Draw(MiiEngine::BlendMode mode = MiiEngine::BlendMode::KBlendModeNormal)override;

	/// <summary>
	/// 生成処理
	/// </summary>
	/// <param name="pos">作成するオブジェクトの位置を表す 3 次元ベクトル（const 参照）。</param>
	/// <param name="direction">オブジェクトの向きまたは進行方向を表す 3 次元ベクトル（const 参照）。</param>
	/// <param name="player">プレイヤーオブジェクトへのポインタ。生成された弾がプレイヤーに対してどのように振る舞うかを制御するために使用されます。</param>
	void Create(const Vector3& pos, const Vector3& direction, Player* player);

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information() override {};

public: /// ===衝突判定=== ///
	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="collider">衝突したオブジェクトを表す Collider へのポインタ。衝突処理に使用される対象を示します。</param>
	void OnCollision(MiiEngine::Collider* collider) override;

public: /// ===Getter=== ///
	// 生存フラグの取得
	bool GetIsAlive() const { return isAlive_; };

private: /// ===変数=== ///

	/// ===基本情報=== ///
	struct BaseInfo {
		Vector3 direction = { 0.0f, 0.0f, 0.0f }; // 方向
		Vector3 velocity = { 0.0f, 0.0f, 0.0f }; // ベクトル
	};
	BaseInfo info_;

	// ParticleGroup
	MiiEngine::ParticleGroup* bulletParticle_ = nullptr;

	// Player
	Player* player_ = nullptr;

	// 速度
	float speed_ = 0.2f;
	// 生存タイマー
	float lifeTimer_ = 0.0f;
	// 生存時間
	float lifeTime_ = 10.0f;
	// 生存フラグ
	bool isAlive_ = true;

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

