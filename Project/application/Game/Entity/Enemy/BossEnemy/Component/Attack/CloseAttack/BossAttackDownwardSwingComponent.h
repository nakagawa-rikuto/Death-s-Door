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
		Idle,       // 非アクティブ（待機）
		WindUp,     // 予備動作（上体を後ろへ反らしてタメる）
		Strike,     // 攻撃（前方へ一気に振り下ろす）
		HoldDown,   // 余韻（叩きつけた深い前傾を一瞬維持）
		Recovery,   // 回復（正位置へゆっくり戻る）
		Finished,   // 攻撃終了
	};

	/// ===状態の構造体=== ///
	struct DownwardSwingState {
		Vector3 currentPositino{}; // 現在のワールド座標
		float phaseTimer = 0.0f;   // 現フェーズの経過時間
	};

public:

	/// ===設定パラメータの構造体=== ///
	struct DownwardSwingConfig {
		// --- WindUp（予備動作）---
		// 上体をX軸正方向（後方仰け反り）へ傾ける角度（度）
		float windUpPitch = 15.0f;
		// タメにかける時間（秒）。「じわっ」と遅めに設定するほど重さが出る
		float windUpDuration = 0.35f;

		// --- Strike（振り下ろし）---
		// 振り下ろし先のX軸負方向（前傾）角度（度）。windUpPitch との合計が可動域
		// 例: windUpPitch=15, strikeForwardPitch=15 → 合計30度の可動
		float strikeForwardPitch = 15.0f;
		// 振り下ろし時間（秒）。WindUpより大幅に短くすることで「シュッ」とした鋭さを演出
		float strikeDuration = 0.10f;

		// --- HoldDown（余韻）---
		// 叩きつけ後に深い前傾を維持する時間（秒）。0にすると余韻なし
		float holdDownDuration = 0.08f;

		// --- Recovery（戻り）---
		// 正位置へ戻るまでの時間（秒）。Strike より長めでゆっくり収束させる
		float recoveryDuration = 0.40f;

		// --- 踏み込みオフセット ---
		// Strike 中にボスを前方へ踏み込ませる距離（ローカルZ軸正方向）
		float strikeStepForward = 0.5f;

		// --- 武器オフセット（Bossローカル座標）---
		// 定位置
		Vector3 weaponRestOffset = { 0.0f,  0.5f,  0.3f };
		// タメ時：頭上の少し背中側へ
		Vector3 weaponWindUpOffset = { 0.0f,  1.4f, -0.4f };
		// 振り下ろし先：モデル前方下方
		Vector3 weaponStrikeOffset = { 0.0f, -0.2f,  1.5f };
	};

	/// ===更新用コンテキスト=== ///
	struct UpdateContext {
		Quaternion baseRotation{};  // 攻撃開始時のBoss基底回転
		float deltaTime = 0.0f;     // デルタタイム
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		Vector3 velocity{};             // ボスに適用する移動速度（ワールド座標系）
		Quaternion rotation{};             // モデルに適用する回転（基底回転 + ピッチ）
		Vector3 weaponPosition{};       // ローカル座標系での武器位置オフセット
		DownwardSwingPhase currentPhase{}; // 現在フェーズ
		bool isFinished = false;           // 攻撃が完全終了したか
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
	/// 攻撃を開始する。呼び出した瞬間からWindUpフェーズへ遷移。
	/// </summary>
	void StartAttack();

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
	/// X軸周りに angleDeg 度回転させるクォータニオンを生成する。
	/// 正値: 後方仰け反り（WindUp方向）
	/// 負値: 前方前傾（Strike方向）
	/// </summary>
	Quaternion MakePitchQuaternion(float angleDeg) const;
};

