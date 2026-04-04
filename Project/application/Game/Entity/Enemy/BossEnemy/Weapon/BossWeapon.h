#pragma once
/// ===Include=== ///
// OBB
#include <Engine/Collider/OBBCollider.h>

/// ===前方宣言=== ///
class BossEnemy;

///=====================================================/// 
/// BossWeapon
///=====================================================///
class BossWeapon : public MiiEngine::OBBCollider {
public:

	BossWeapon() = default;
	~BossWeapon() override;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="mode">描画に使用するブレンドモード。</param>
	void Draw(MiiEngine::BlendMode mode) override;

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information() override;

	/// <summary>
	/// 親のボスエネミーを設定します。
	/// </summary>
	/// <param name="parent">親として設定するボスエネミーへのポインター。</param>
	void SetUpParent(BossEnemy* parent);

public: /// ===衝突処理=== ///
	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="collider">衝突した相手の Collider オブジェクトを指すポインター。</param>
	void OnCollision(MiiEngine::Collider* collider) override;

public: /// ===Getter=== ///

	/// <summary>
	/// ワールド座標系での平行移動を取得します。
	/// </summary>
	/// <returns>ワールド座標系での平行移動を表すVector3への定数参照。</returns>
	const Vector3& GetWorldTranslate() const { return object3d_->GetWorldTranslate(); }

public: /// ===Setter=== ///
	// アクティブ状態の設定
	void SetActive(bool flag);

private: /// ===メンバ変数=== ///
	// BossEnemy
	BossEnemy* bossEnemy_ = nullptr;
	// Colliderのアクティブ
	bool isActive_ = false;
};

