#pragma once
/// ===Include=== ///
#include <Math/Vector3.h>
#include <Math/Vector4.h>

///=====================================================/// 
/// BossHitReactionComponent
/// 
///=====================================================///
class BossHitReactionComponent {
private:
	/// ===状態の構造体=== ///
	struct KnockBackState {
		float slowdownTimer = 0.0f;		 // 減速用タイマー
		float slowdownMultiplier = 1.0f; // 減速率の乗数
		bool isHit = false;				 // ヒットしたかどうか
	};
	/// ===色の状態構造体=== ///
	struct ColorState {
		float colorTimer = 0.0f;    // 色変化用タイマー
		float colorInterval = 0.0f; // 色変化の間隔
		bool isColorChange = false; // 色変化フラグ
	};

public:
	/// ===設定パラメータの構造体=== ///
	struct KnockBackConfig {
		float knockBackForce = 1.5f;	// ノックバックの強さ
		float slowdownFactor = 0.3f;	// 減速率(0.0~1.0、小さいほど遅くなる)
		float slowdownDuration = 0.2f;	// 減速効果の持続時間
		float alphaDuration = 0.2f;		// 透明度変化の持続時間
		float hitAlpha = 0.6f;			// ヒット時の透明度
		float flashSpeed = 10.0f;		// 点滅速度
	};

	/// ===更新用コンテキスト=== ///
	struct UpdateContext {
		Vector3 currentPosition;	// 現在の位置
		Vector3 currentVelocity;	// 現在の速度
		Vector4 currentColor;    // 現在の色
		float deltaTime;
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		Vector3 velocity = { 0.0f, 0.0f, 0.0f };
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

public:

	BossHitReactionComponent() = default;
	~BossHitReactionComponent() = default;

	// コピー・ムーブは明示的に制御
	BossHitReactionComponent(const BossHitReactionComponent&) = delete;
	BossHitReactionComponent& operator=(const BossHitReactionComponent&) = delete;

	/// <summary>
	/// KnockBack の初期化を行います。
	/// </summary>
	/// <param name="config">初期化に使用する設定を const 参照で受け取ります。引数を省略した場合は KnockBackConfig の既定値が使用されます。</param>
	void Initialize(const KnockBackConfig& config = KnockBackConfig{});

	/// <summary>
	/// 与えられた更新コンテキストに基づいて更新処理を行い、その結果を返します。
	/// </summary>
	/// <param name="context">更新に必要な情報を保持する入力コンテキスト。参照で渡され、関数はこの情報を用いて更新を実行します。</param>
	/// <returns>更新処理の結果を表す UpdateResult。成功／失敗や状態に関する情報を含みます。</returns>
	UpdateResult Update(const UpdateContext& context);

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information();

	/// <summary>
	/// オブジェクトがヒットされたときに呼び出されるコールバック。ヒットの方向を受け取ります。
	/// </summary>
	/// <param name="hitParentDirection">親座標系（親オブジェクト基準）で表されたヒットの方向を示すVector3へのconst参照。関数内で値は変更されません。</param>
	void OnHit(const Vector3& hitParentDirection);

public: /// ===Getter=== ///
	// KnockBackConfig
	const KnockBackConfig& GetConfig() const { return config_; }
	// KnockBackState
	const KnockBackState& GetKnockBackState() const { return state_; }
	// ColorState
	const ColorState& GetColorState() const { return colorState_; }

#ifdef USE_IMGUI
public: /// ===Setter=== ///
	// KnockBackConfig
	void ApplyConfig(const KnockBackConfig& newConfig);
#endif // USE_IMGUI
private:
	/// ===情報=== ///
	KnockBackConfig config_;
	KnockBackState state_;
	ColorState colorState_;

	// ヒットした相手の方向
	Vector3 hitParentDirection_ = { 0.0f, 0.0f, 0.0f };

private:

	/// <summary>
	/// タイマーの更新処理
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	void TimerUpdate(const float deltaTime);

	/// <summary>
	/// 減速係数の更新処理
	/// </summary>
	void UpdateSlowdownMultiplier();

	/// <summary>
	/// 渡された色を元に更新された色を返します。`
	/// </summary>
	/// <param name="color">更新の基となる色。参照渡しされた const Vector4 オブジェクト。</param>
	/// <returns>更新された色を表す Vector4 オブジェクト。</returns>
	Vector4 UpdateColor(const Vector4& color) const;
};
