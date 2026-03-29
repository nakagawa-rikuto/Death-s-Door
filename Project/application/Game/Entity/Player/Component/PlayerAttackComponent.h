#pragma once
/// ===Include=== ///
#include "application/Game/Editor/Data/AttackData.h"
// C++
#include <memory>
#include <unordered_map>
#include <string>

/// ===前方宣言=== ///
class PlayerHand;
class PlayerWeapon;

///-------------------------------------------/// 
/// PlayerAttackComponent
///-------------------------------------------///
class PlayerAttackComponent {
private:
	/// ===攻撃状態の構造体=== ///
	struct AttackState {
		float timer = 0.0f;         // 攻撃タイマー
		float comboTimer = 0.0f;    // コンボタイマー
		bool isActive = false;      // 攻撃中フラグ
		bool canCombo = false;      // コンボ可能フラグ
		int currentAttackID = -1;   // 現在の攻撃ID
		int previousAttackID = -1;  // 前の攻撃ID
		int comboCount = 0;         // コンボカウント
	};

public:

	PlayerAttackComponent() = default;
	~PlayerAttackComponent() = default;

	// コピー・ムーブは明示的に制御
	PlayerAttackComponent(const PlayerAttackComponent&) = delete;
	PlayerAttackComponent& operator=(const PlayerAttackComponent&) = delete;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const float deltaTime);

	/// <summary>
	/// ImGui情報の表示
	/// </summary>
	void Information();

	/// <summary>
	/// 攻撃データをJSONから読み込み
	/// </summary>
	/// <param name="attackID">攻撃ID</param>
	/// <param name="filepath">JSONファイルパス</param>
	/// <returns>読み込み成功したらtrue</returns>
	bool LoadAttackData(int attackID, const std::string& filepath);

	/// <summary>
	/// 攻撃を開始
	/// </summary>
	/// <param name="attackID">実行する攻撃ID</param>
	/// <param name="weapon">使用する武器</param>
	/// <param name="playerPosition">プレイヤーの位置</param>
	/// <param name="playerRotation">プレイヤーの回転</param>
	/// <returns>攻撃開始に成功したらtrue</returns>
	bool StartAttack(int attackID, PlayerWeapon* weapon, PlayerHand* rightHand, PlayerHand* leftHand);

	/// <summary>
	/// コンボ攻撃を試行
	/// </summary>
	/// <param name="weapon">使用する武器</param>
	/// <param name="playerPosition">プレイヤーの位置</param>
	/// <param name="playerRotation">プレイヤーの回転</param>
	/// <returns>コンボ成功したらtrue</returns>
	bool TryCombo(PlayerWeapon* weapon, PlayerHand* rightHand, PlayerHand* leftHand);

	/// <summary>
	/// 攻撃をキャンセル
	/// </summary>
	void CancelAttack();

public: /// ===Getter=== ///

	// 現在の攻撃データを取得
	const AttackData* GetCurrentAttackData() const;
	// 攻撃データをIDで取得
	const AttackData* GetAttackData(int attackID) const;
	// 攻撃状態
	const AttackState& GetState() const { return state_; }
	// 攻撃中か
	bool IsAttacking() const { return state_.isActive; }
	// コンボ可能か
	bool CanCombo() const { return state_.canCombo; }
	// 現在の攻撃進行度
	float GetAttackProgress() const;

private:
	// 攻撃データの管理
	std::unordered_map<int, AttackData> attackDataMap_;

	// 攻撃状態
	AttackState state_;

private:

	/// <summary>
	/// タイマーを更新します。
	/// </summary>
	/// <param name="deltaTime">前回の更新からの経過時間（秒）</param>
	void UpdateTimers(const float deltaTime);

	/// <summary>
	/// 武器に攻撃軌道を設定
	/// </summary>
	/// <param name="data">適用する攻撃情報を保持する参照。ダメージ値や攻撃種別、エフェクトなどを含みます。</param>
	/// <param name="weapon">攻撃を受ける対象の武器オブジェクトを指すポインタ。</param>
	/// <param name="basePosition">攻撃適用時の基準となる位置（ワールド座標）。</param>
	/// <param name="baseRotation">攻撃適用時の基準となる回転（クォータニオン）。</param>
	void ApplyAttackToWeapon(const AttackData& data, PlayerWeapon* weapon, PlayerHand* rightHand, PlayerHand* leftHand);
};

