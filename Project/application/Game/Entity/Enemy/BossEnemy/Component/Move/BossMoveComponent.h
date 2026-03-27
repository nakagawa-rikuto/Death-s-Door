#pragma once
/// ===Include=== ///
#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

///=====================================================/// 
/// BossMoveComponent
///=====================================================///
class BossMoveComponent {
private:

	/// ===状態の構造体=== ///
	struct MoveState {
		Vector3 direction{};         // 現在の移動方向
	};
public:
	/// ===設定パラメータの構造体=== ///
	struct MoveConfig {
		float speed = 0.05f;			// 移動速度
		float rotationSpeed = 0.1f;		// 回転速度
	};

	/// ===更新用コンテキスト=== ///
	struct UpdateContext {
		Vector3 currentPosition{};		// 現在の位置
		Quaternion currentRotation{};	// 現在の回転
		Vector3 playerPosition{};		// プレイヤーの位置
		float deltaTime = 0.0f;			// デルタタイム
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		Vector3 velocity{};				// 移動ベクトル
		Quaternion rotate{};			// 回転
	};

public:

	BossMoveComponent() = default;
	~BossMoveComponent() = default;

	// コピー・ムーブは明示的に制御
	BossMoveComponent(const BossMoveComponent&) = delete;
	BossMoveComponent& operator=(const BossMoveComponent&) = delete;

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
	};

private:

	/// <summary>
	/// 現在の回転から目標方向への回転を計算します。
	/// </summary>
	/// <param name="current">現在の回転を表すクォータニオン。</param>
	/// <param name="targetDir">目標となる方向ベクトル。</param>
	/// <param name="deltaTime">前フレームからの経過時間（秒）。</param>
	/// <returns>計算された回転を表すクォータニオン。</returns>
	Quaternion CalcRotation(const Quaternion& current, const Vector3& targetDir, float deltaTime) const;

	/// <summary>
	/// 2つの方向ベクトル間の回転を表すクォータニオンを計算します。
	/// </summary>
	/// <param name="from">回転の開始方向ベクトル。</param>
	/// <param name="to">回転の終了方向ベクトル。</param>
	/// <returns>from方向からto方向への回転を表すクォータニオン。</returns>
	Quaternion DirectionToQuaternion(const Vector3& from, const Vector3& to) const;
};

