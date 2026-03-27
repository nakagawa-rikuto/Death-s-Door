#pragma once
/// ===Include=== ///
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

///=====================================================/// 
/// JumpSmashComponent
/// ジャンプして攻撃する処理
///=====================================================///
class BossAttackJumpSmashComponent {
private:
	/// ===フェーズ定義=== ///
	enum class LeapPhase {
		Idle,        // 非アクティブ（待機）
		LeapWindUp,  // 跳躍前の溜め（屈んで重心を落とす）
		Leap,        // 放物線飛行
		Strike,      // 着地叩きつけ
		HoldDown,    // 叩きつけ余韻（深い前傾を一瞬維持）
		Recovery,    // 正位置へ戻る
		Finished,    // 攻撃終了
	};

	/// ===状態の構造体=== ///
	struct JumpSmashState {
		float phaseTimer = 0.0f;   // 現フェーズの経過時間
		Quaternion baseRotation{};  // 攻撃開始時の基底回転

		// --- Leap フェーズで使用する飛行データ ---
		Vector3 startPosition{};    // 跳躍開始位置（ワールド座標）
		Vector3 targetPosition{};   // 着地目標位置（ワールド座標）
		Vector3 currentPosition{};  // 現在のワールド座標（毎フレーム更新）
	};

public:

	/// ===設定パラメータの構造体=== ///
	struct JumpSmashConfig {
		// LeapWindUp
		// 屈み時のY軸スケール縮小量（見た目の「しゃがみ」演出用。スケールは呼出し側で適用）
		float leapWindUpCrouchPitch = -10.0f;  // 屈み時の前傾角度（度）。前のめりにしゃがむ
		float leapWindUpDuration = 0.30f;  // 溜め時間（秒）

		// Leap
		float leapDuration = 0.60f;  // 飛行にかける時間（秒）
		float leapArcHeight = 4.0f;   // 放物線の頂点高さ（ワールドY単位）

		// 飛行中のピッチ変化
		float leapAscentPitch = 20.0f;  // 上昇中の前傾角度（度）
		float leapDescentPitch = 30.0f;  // 降下中に仰け反る角度（度）。着地直前の「タメ」

		// Strike
		float strikeForwardPitch = 25.0f;  // 着地時の前傾角度（度）
		float strikeDuration = 0.10f; // 叩きつけ時間（秒）

		// HoldDown
		float holdDownDuration = 0.08f;  // 前傾維持時間（秒）。0 でスキップ

		// Recovery
		float recoveryDuration = 0.45f;  // 正位置へ戻る時間（秒）

		// 武器オフセット
		Vector3 weaponRestOffset = { 0.0f,  0.5f,  0.3f };  // 定位置
		Vector3 weaponLeapOffset = { 0.0f,  1.5f, -0.5f };  // 飛行中（振り上げ）
		Vector3 weaponStrikeOffset = { 0.0f, -0.2f,  1.5f };  // 叩きつけ先
	};

	/// ===更新用コンテキスト=== ///
	struct UpdateContext {
		Vector3 currentPosition{};  // Bossの現在位置
		float deltaTime = 0.0f;		// デルタタイム
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		Vector3 velocity{};          // 今フレームの移動量（呼び出し側が position += velocity で適用する）
		Quaternion rotation{};       // モデルに適用する回転（基底回転 + ピッチ）
		Vector3 weaponPosition{};    // ローカル座標系での武器位置オフセット
		LeapPhase currentPhase{};    // 現在フェーズ
		bool isFinished = false;     // 攻撃が完全終了したか
	};

public:

	BossAttackJumpSmashComponent() = default;
	~BossAttackJumpSmashComponent() = default;

	BossAttackJumpSmashComponent(const BossAttackJumpSmashComponent&) = delete;
	BossAttackJumpSmashComponent& operator=(const BossAttackJumpSmashComponent&) = delete;

	/// <summary>
	/// 初期化（コンポーネント生成時に一度だけ呼ぶ）
	/// </summary>
	void Initialize(const JumpSmashConfig& config = JumpSmashConfig{});

	/// <summary>
	/// 更新処理。毎フレーム呼び出す。
	/// </summary>
	UpdateResult Update(const UpdateContext& context);

	/// <summary>
	/// 攻撃を強制リセットしIdleへ戻す。
	/// </summary>
	void Reset();

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information();

	/// <summary>
	/// 攻撃を開始する。
	/// </summary>
	/// <param name="bossPosition">攻撃開始時のBossのワールド座標</param>
	/// <param name="playerPosition">着地目標となるPlayerのワールド座標</param>
	/// <param name="baseRotation">攻撃開始時のBossの基底回転</param>
	void StartAttack(
		const Vector3& bossPosition,
		const Vector3& playerPosition,
		const Quaternion& baseRotation
	);

public: /// ===Getter=== ///
	LeapPhase GetPhase() const { return phase_; }
	bool IsActive() const { return phase_ != LeapPhase::Idle && phase_ != LeapPhase::Finished; }
	bool IsFinished() const { return phase_ == LeapPhase::Finished; }
	const JumpSmashConfig& GetConfig() const { return config_; }
	const Vector3& GetPosition() const { return state_.currentPosition; }

#ifdef USE_IMGUI
public: /// ===Setter=== ///
	void ApplyConfig(const JumpSmashConfig& newConfig);
#endif // USE_IMGUI

private:
	/// ===情報=== ///
	JumpSmashConfig config_{};
	JumpSmashState  state_{};
	LeapPhase  phase_ = LeapPhase::Idle;

private:
	/// <summary>攻撃の更新を実行します。</summary>
	void UpdateAttack(const UpdateContext& context, UpdateResult& result);

	/// <summary>LeapWindUp フェーズの更新。</summary>
	void UpdateLeapWindUp(UpdateResult& result);

	/// <summary>Leap フェーズの更新。放物線移動とピッチ変化を処理する。</summary>
	void UpdateLeap(UpdateResult& result);

	/// <summary>Strike フェーズの更新。</summary>
	void UpdateStrike(UpdateResult& result);

	/// <summary>HoldDown フェーズの更新。</summary>
	void UpdateHoldDown(UpdateResult& result);

	/// <summary>Recovery フェーズの更新。</summary>
	void UpdateRecovery(UpdateResult& result);

	/// <summary>
	/// 放物線の高さを計算する。
	/// </summary>
	float CalcParabolaHeight(float t) const;

	/// <summary>X軸周りに angleDeg 度回転させるクォータニオンを生成する。</summary>
	Quaternion MakePitchQuaternion(float angleDeg) const;
};

