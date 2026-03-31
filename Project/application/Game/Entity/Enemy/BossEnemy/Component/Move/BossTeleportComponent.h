#pragma once
/// ===Include=== ///
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <random>

///=====================================================/// 
/// TeleportComponent
///=====================================================///
class BossTeleportComponent {
private:
	/// ===テレポートフェーズ=== ///
	enum class Phase {
		None,     // 演出なし
		SpinOut,  // その場でEaseIn回転しながら透明化（移動なし）
		Warp,     // 完全透明のまま NextPosition へ座標をワープ（1フレーム）
		SpinIn,   // NextPosition でEaseOut回転しながら不透明化（移動なし）
	};

	/// ===状態の構造体=== ///
	struct TeleportState {
		Phase phase = Phase::SpinOut;
		float phaseTimer = 0.0f;		// フェーズ内の経過時間
		float alpha = 1.0f;				// 透明度（1=不透明, 0=透明）
		Quaternion startRotation{};     // 各フェーズ開始時の回転（スピンの基点）
		Quaternion rotation{};          // スピン込みの現在の回転
		Vector3 warpStartPosition{};	// ワープ開始時の座標（SpinOut の回転の中心）
		Vector3 nextPosition{};			// ワープ先座標
	};

public:

	/// ===設定パラメータの構造体=== ///
	struct TeleportConfig {
		float rotationSpeed = 5.0f;		// 向き補間速度
		float spinOutDuration = 0.6f;   // SpinOut の時間（回転しながら透明化）
		float warpDuration = 0.1f;      // Warp の時間（完全透明のままワープする時間）
		float spinInDuration = 0.6f;	// SpinIn  の時間（回転しながら不透明化）
	};

	/// ===更新用コンテキスト=== ///
	struct UpdateContext {
		Vector3 currentPosition{};		// 現在の座標
		Quaternion currentRotation{};	// 現在の回転
		Vector3 playerPosition{};		// プレイヤーの座標
		float deltaTime = 0.0f;			// デルタタイム
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		Vector3 velocity{};              // 移動ベクトル（フレーム移動量）
		Vector3 targetDirection{};       // 目標方向（State 側で回転処理に使用）
		Quaternion rotation{};			 // テレポート用の追加回転（SpinOut/SpinInで使用）
		float colorAlpha = 1.0f;		 // 透明度（描画側で適用）

		bool needsRotation = false;			   // 回転処理が必要かどうか
		bool shouldResetRotationFlag = false;  // 回転完了フラグをリセットする必要があるか

		bool shouldSpawnParticleAtCurrent = false;  // 現在地にパーティクルを出すか
		bool shouldSpawnParticleAtNext = false;     // nextPosition にパーティクルを出すか

		bool isComplete = false;  // 演出完了フラグ
	};

public:

	BossTeleportComponent() = default;
	~BossTeleportComponent() = default;

	BossTeleportComponent(const BossTeleportComponent&) = delete;
	BossTeleportComponent& operator=(const BossTeleportComponent&) = delete;

	/// <summary>
	/// テレポート設定を使用して初期化を行います。
	/// </summary>
	/// <param name="config">使用するテレポート設定。デフォルト値が提供されています。</param>
	void Initialize(const TeleportConfig& config = TeleportConfig{});

	/// <summary>
	/// 更新処理を実行します。
	/// </summary>
	/// <param name="context">更新処理に使用するコンテキスト。</param>
	/// <returns>更新処理の結果。</returns>
	UpdateResult Update(const UpdateContext& context);

	/// <summary>
	/// ImGui デバッグ表示。
	/// </summary>
	void Information();

	/// <summary>
	/// テレポートを開始します
	/// </summary>
	/// <param name="currentPosition"></param>
	/// <param name="currentRotation"></param>
	/// <param name="playerPosition"></param>
	/// <param name="minRange"></param>
	/// <param name="maxRange"></param>
	void Start(
		const Vector3& currentPosition,
		const Quaternion& currentRotation,
		const Vector3& playerPosition,
		float minRange, float maxRange);


public: /// ===Getter=== ///
	const TeleportConfig& GetConfig() const { return config_; }
	Phase GetPhase()  const { return state_.phase; }
	float GetAlpha()  const { return state_.alpha; }

#ifdef USE_IMGUI
public: /// ===Setter=== ///
	void ApplyConfig(const TeleportConfig& config) { config_ = config; }
#endif

private:
	/// ===情報=== ///
	TeleportConfig config_;
	TeleportState  state_;

	/// ===ランダムエンジン=== ///
	std::mt19937 randomEngine_;

private:
	/// <summary>
	/// 次の位置を設定します。
	/// </summary>
	/// <param name="minRange">最小範囲。</param>
	/// <param name="maxRange">最大範囲。</param>
	/// <param name="currentPosition">現在の位置。</param>
	/// <returns>次の位置を表すVector3。</returns>
	Vector3 SetNextPosition(float minRange, float maxRange, const Vector3& currentPosition);

	/// <summary>
	/// テレポートを更新します。
	/// </summary>
	/// <param name="context">更新コンテキスト。</param>
	void UpdateTeleport(const UpdateContext& context, UpdateResult& result);

	/// <summary>
	/// Y 軸周りに angleRad だけ回転する Quaternion を生成します。
	/// </summary>
	static Quaternion MakeRotationY(float angleRad);

};

