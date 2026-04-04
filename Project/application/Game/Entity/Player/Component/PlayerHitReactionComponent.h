#pragma once
/// ===Include=== ///
#include <Math/Vector3.h>

///=====================================================/// 
/// PlayerHitReactionComponent（ヒットリアクション）
///=====================================================///
class PlayerHitReactionComponent {
public:
	struct HitConfig {
		float knockbackSpeed = 1.5f; // ノックバックの速度
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		Vector3 velocity;		    // 移動ベクトル
	};

public:

	PlayerHitReactionComponent() = default;
	~PlayerHitReactionComponent() = default;

	// コピー・ムーブは明示的に制御
	PlayerHitReactionComponent(const PlayerHitReactionComponent&) = delete;
	PlayerHitReactionComponent& operator=(const PlayerHitReactionComponent&) = delete;

	/// <summary>
	/// 初期化を実行します。
	/// </summary>
	/// <param name="config">初期化の構成設定。デフォルトではHitConfig{}が使用されます。</param>
	void Initialize(const HitConfig& config = HitConfig{});

	/// <summary>
	/// ヒットリアクションを開始します。
	/// </summary>
	/// <param name="knockbackDirection">ノックバックの方向。</param>
	UpdateResult Start(const Vector3& knockbackDirection);

	/// <summary>
	/// ImGui情報
	/// </summary>
	void Information();

public: /// ===Getter=== ///
	// HitConfig
	const HitConfig& GetConfig() const { return config_; }

private:
	/// ===情報=== ///
	HitConfig config_;
	Vector3 currentKnockBackDirection_;
};

