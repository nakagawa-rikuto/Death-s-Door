#pragma once
/// ===Include=== ///
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

///=====================================================/// 
/// DownwardSwing
/// 振り下ろし攻撃
///=====================================================///
class BossAttackDownwardSwingComponent {
private:

	/// ===フェーズ定義=== ///
	enum class DownwardSwingPhase {
		Idle,       // 非アクティブ
		WindUp,     // 予備動作
		Strike,     // 攻撃
		HoldDown,   // 余韻
		Recovery,   // 回復
		Finished,   // 攻撃終了
	};

	/// ===状態の構造体=== ///
	struct DownwardSwingState {
		Vector3 currentPosition{}; // 現在のワールド座標
		float phaseTimer = 0.0f;   // 現フェーズの経過時間
		Quaternion baseRotation{};  // 攻撃開始時のBoss基底回転
	};

public:

	/// ===設定パラメータの構造体=== ///
	struct DownwardSwingConfig {
		// --- WindUp---
		// 上体をX軸正方向へ傾ける角度
		float windUpPitch = 15.0f;
		// タメにかける時間
		float windUpDuration = 0.35f;

		// --- Strike---
		// 振り下ろし先のX軸負方向角度
		float strikeForwardPitch = 15.0f;
		// 振り下ろし時間
		float strikeDuration = 0.10f;

		// --- HoldDown---
		// 叩きつけ後に深い前傾を維持する時間
		float holdDownDuration = 0.08f;

		// --- Recovery---
		// 正位置へ戻るまでの時間
		float recoveryDuration = 0.40f;

		// --- 踏み込みオフセット ---
		// Strike中にボスを前方へ踏み込ませる距離
		float strikeStepForward = 0.5f;

		// --- 武器オフセット ---
		// 定位置
		Vector3 weaponRestOffset = { 0.0f,  0.5f,  0.3f };
	};

	/// ===更新用コンテキスト=== ///
	struct UpdateContext {
		Quaternion currentRotation{};  // 攻撃開始時のBoss基底回転
		float deltaTime = 0.0f;     // デルタタイム
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		Vector3 velocity{};             // ボスに適用する移動速度
		Quaternion rotation{};          // モデルに適用する回転
		Vector3 weaponPosition{};       // ローカル座標系での武器位置オフセット
		bool isAttacking = false;       // 攻撃判定が有効なフレーム
		bool isFinished = false;        // 攻撃が完全終了したか
		bool onStrike = false;          // 踏み込み/攻撃開始の瞬間
	};

public:

	BossAttackDownwardSwingComponent() = default;
	~BossAttackDownwardSwingComponent() = default;

	BossAttackDownwardSwingComponent(const BossAttackDownwardSwingComponent&) = delete;
	BossAttackDownwardSwingComponent& operator=(const BossAttackDownwardSwingComponent&) = delete;

	/// <summary>
	/// 初期化（コンポーネント生成時に一度だけ呼ぶ）
	/// </summary>
	void Initialize(const DownwardSwingConfig& config = DownwardSwingConfig{});

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
	/// 指定された回転で攻撃を開始します。
	/// </summary>
	/// <param name="rotate">攻撃の回転を表すクォータニオン。</param>
	void StartAttack(const Quaternion& rotate);

public: /// ===Getter=== ///
	DownwardSwingPhase GetPhase()  const { return phase_; }
	bool IsActive()  const { return phase_ != DownwardSwingPhase::Idle && phase_ != DownwardSwingPhase::Finished; }
	bool IsFinished()const { return phase_ == DownwardSwingPhase::Finished; }
	const DownwardSwingConfig& GetConfig() const { return config_; }

#ifdef USE_IMGUI
public: /// ===Setter=== ///
	void ApplyConfig(const DownwardSwingConfig& newConfig);
#endif // USE_IMGUI

private:
	/// ===情報=== ///
	DownwardSwingConfig config_{};
	DownwardSwingState state_{};
	DownwardSwingPhase phase_ = DownwardSwingPhase::Idle;

private:
	/// <summary>
	/// 攻撃の更新を実行します。
	/// </summary>
	void UpdateAttack(const UpdateContext& context, UpdateResult& result);

	/// <summary>
	/// ワインドアップを更新します。
	/// </summary>
	/// <param name="context">更新コンテキスト。</param>
	/// <param name="result">更新結果を格納する出力パラメータ。</param>
	void UpdateWindUp(const UpdateContext& context, UpdateResult& result);

	/// <summary>
	/// ストライク情報を更新します。
	/// </summary>
	/// <param name="context">更新処理に必要なコンテキスト情報。</param>
	/// <param name="result">更新結果を格納する出力パラメータ。</param>
	void UpdateStrike(const UpdateContext& context, UpdateResult& result);

	/// <summary>
	/// ホールドダウンの状態を更新します。
	/// </summary>
	/// <param name="context">更新に必要な情報を含むコンテキスト。</param>
	/// <param name="result">ホールドダウンの状態で更新される結果オブジェクト。</param>
	void UpdateHoldDown(const UpdateContext& context, UpdateResult& result);

	/// <summary>
	/// リカバリー処理を更新します。
	/// </summary>
	/// <param name="context">更新処理のコンテキスト情報。</param>
	/// <param name="result">更新結果を格納する出力パラメータ。</param>
	void UpdateRecovery(const UpdateContext& context, UpdateResult& result);

	/// <summary>
	/// X軸周りに angleDeg 度回転させるクォータニオンを生成する。
	/// </summary>
	Quaternion MakePitchQuaternion(float angleDeg) const;
};

