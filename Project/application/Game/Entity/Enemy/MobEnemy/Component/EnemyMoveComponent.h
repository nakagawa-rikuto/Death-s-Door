#pragma once
/// ===Include=== ///
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <random>

///=====================================================/// 
/// EnemyMoveComponent
///=====================================================///
class EnemyMoveComponent {
private:
	/// ===状態の構造体=== ///
	struct MoveState {
		float moveTimer = 0.0f;		// 1回の移動残り時間（moveTime からカウントダウン）
		float evadeTimer = 0.0f;	// 回避圏内の猶予タイマー（evadeTime からカウントダウン）

		Vector3 direction{};         // 現在の移動方向（正規化済み）

		bool isInEvadeRange = false; // プレイヤーがevadeRange内にいるかどうか
		bool isInChaseRange = false; // プレイヤーがchaseRange内にいるかどうか
	};
public:
	/// ===設定パラメータの構造体=== ///
	struct MoveConfig {
		// 速度
		float speed = 0.05f;			// 速度
		float rotationSpeed = 0.1f;		// 回転速度
		// 範囲
		float chaseRange = 20.0f;		// 追いかける範囲
		float evadeRange = 3.0f;		// 距離を取る範囲
		// 時間
		float moveTime = 0.0f;			// 移動時間
		float evadeTime = 0.0f;			// 回避時間
	};

	/// ===更新用コンテキスト=== ///
	struct UpdateContext {
		Vector3 currentPosition{};		 // 現在の位置
		Vector3 playerPosition{};		 // プレイヤーの位置
		float deltaTime = 0.0f;			 // デルタタイム
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		Vector3 velocity{};				// 移動ベクトル
		Vector3 rotateDirection{};		// 目標方向
		bool teleportTrigger = false;	// テレポートトリガー
	};

public:

	EnemyMoveComponent() = default;
	~EnemyMoveComponent() = default;

	// コピー・ムーブは明示的に制御
	EnemyMoveComponent(const EnemyMoveComponent&) = delete;
	EnemyMoveComponent& operator=(const EnemyMoveComponent&) = delete;

	/// <summary>
	/// 指定した敵オブジェクトを初期化し、中心位置を設定します。
	/// </summary>
	void Initialize(const MoveConfig& config = MoveConfig{});

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="context">更新処理に必要なコンテキスト情報。</param>
	/// <returns>更新処理の実行結果。</returns>
	UpdateResult Update(const UpdateContext& context);

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information();

public: /// ===Getter=== ///
	// MoveConfig
	const MoveConfig& GetConfig() const { return config_; }
	// MoveState
	const MoveState& GetState() const { return state_; }

#ifdef USE_IMGUI
public: /// ===Setter=== ///
	// MoveConfig
	void ApplyConfig(const MoveConfig& newconfig);
#endif // USE_IMGUI

private:
	/// ===情報=== ///
	MoveConfig config_;
	MoveState state_;

	/// ===状態=== ///
	enum class MoveStateType {
		Idle,   // 待機
		Chase,  // 追跡
		Evade,  // 回避
	};

	/// ===ランダムシード=== ///
	std::mt19937 randomEngine_;

private:

	/// <summary>
	/// 移動をセットアップします。
	/// </summary>
	void SetupRandomMove();

	/// <summary>
	/// タイマーの更新処理
	/// </summary>
	/// 
	void TimerUpdate(const float deltaTime);
};

