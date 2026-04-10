#pragma once
/// ===Include=== ///
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

///=====================================================/// 
/// Rotate
/// （回転）攻撃コンポーネント
///=====================================================///
class BossAttackRotateComponent {
private:
	/// ===フェーズ定義=== ///
	enum class RotatePhase {
		Idle,       // 非アクティブ（待機）
		WindUp,     // 予備動作（体をひねってタメる）
		Strike,     // 攻撃（切り返して武器を突き出す）
		Recovery,   // 回復（-strikeAngle → 0° へゆっくり戻る）
		Finished,   // 攻撃終了
	};

	/// ====状態の構造体== ///
	struct RotateState {
		float phaseTimer = 0.0f;   // 現フェーズの経過時間
	};
public:
	/// ===設定パラメータの構造体=== ///
	struct RotateConfig {
		// --- WindUp（予備動作）---
		float windUpAngle = 30.0f;  // タメ時にY軸回転させる角度（度）
		float windUpDuration = 0.25f;  // タメにかける時間（秒）

		// --- Strike（突き）---
		float strikeAngle = 15.0f;  // 切り返しのY軸回転角度（度）正面を超えた反対側まで（度）
		float strikeDuration = 0.12f;  // 突き動作にかける時間（秒）

		// --- Recovery（戻り）---
		float recoveryDuration = 0.35f;  // 0° へ戻るまでの時間（秒）。Strike より長めでゆっくり戻る

		// --- 武器オフセット ---
		// 武器の初期位置オフセット（Bossローカル座標）
		Vector3 weaponRestOffset = { 0.8f,  0.0f,  0.0f };
		// タメ時の武器引き絞りオフセット（Bossローカル座標）
		Vector3 weaponWindUpOffset = { 0.8f,  0.0f, -0.3f };
		// 突き時の武器前方オフセット（Bossローカル座標）
		Vector3 weaponStrikeOffset = { 0.4f,  0.0f,  1.2f };
	};

	/// ===更新用コンテキスト=== ///
	struct UpdateContext {
		Quaternion baseRotation{};  // 攻撃開始時のBoss基底回転
		float deltaTime = 0.0f;		// デルタタイム
	};

	/// ===更新結果=== ///
	struct UpdateResult {
		Quaternion modelRotation{};  // モデルに適用する回転（基底回転 + ひねり）
		Vector3 weaponLocalOffset{}; // ローカル座標系での武器位置オフセット
		bool isAttacking = false;    // 攻撃判定が有効なフレーム
		bool isFinished = false;	 // 攻撃が完全終了したか
	};

public:

	BossAttackRotateComponent() = default;
	~BossAttackRotateComponent() = default;

	BossAttackRotateComponent(const BossAttackRotateComponent&) = delete;
	BossAttackRotateComponent& operator=(const BossAttackRotateComponent&) = delete;

	/// <summary>
	/// 初期化（コンポーネント生成時に一度だけ呼ぶ）
	/// </summary>
	void Initialize(const RotateConfig& config = RotateConfig{});

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
	RotatePhase GetPhase()  const { return phase_; }
	bool IsActive()  const { return phase_ != RotatePhase::Idle && phase_ != RotatePhase::Finished; }
	bool IsFinished()const { return phase_ == RotatePhase::Finished; }
	const RotateConfig& GetConfig() const { return config_; }

#ifdef USE_IMGUI
public: /// ===Setter=== ///
	void ApplyConfig(const RotateConfig& newConfig);
#endif // USE_IMGUI

private:
	/// ===情報=== ///
	RotateConfig config_{};
	RotateState  state_{};
	RotatePhase  phase_ = RotatePhase::Idle;

private:

	/// <summary>
	/// 攻撃の更新を実行します。
	/// </summary>
	/// <param name="context">更新のコンテキスト情報。</param>
	/// <param name="result">更新結果を格納する出力パラメータ。</param>
	void UpdateAttack(const UpdateContext& context, UpdateResult& result);

	/// <summary>
	/// Y軸周りに angle 度回転させるクォータニオンを生成する。
	/// </summary>
	Quaternion MakeYawQuaternion(float angleDeg) const;

	/// <summary>
	/// 2つのベクトルをtで線形補間する。
	/// </summary>
	Vector3 LerpVector3(const Vector3& a, const Vector3& b, float t) const;
};

