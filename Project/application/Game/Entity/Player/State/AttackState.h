/// ===Include=== ///
#include "Base/PlayerState.h"
// AttackData
#include "Engine/System/Editor/Attack/Data/AttackData.h"
// Math
#include "Math/Vector3.h"

/// ===前方宣言=== ///
class PlayerWeapon;

///=====================================================/// 
/// AttackState
///=====================================================///
class AttackState : public PlayerState {
public:

	~AttackState() override = default;

	/// <summary>
	/// ステートに入った時に呼ばれる処理
	/// </summary>
	/// <param name="player">状態に入る対象のプレイヤーを指すポインタ。</param>
	/// <param name="camera">状態で使用するゲームカメラを指すポインタ。</param>
	void Enter(Player* player, MiiEngine::CameraCommon* camera) override;

	/// <summary>
	/// ステート時の更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// ステートの終了処理
	/// </summary>
	void Finalize() override;

private:
	/// ===状態=== ///
	struct State {
		int attackID = -1;		   // 攻撃ID
		int previousAttackID = -1; // 前の攻撃ID
		int comboCount = 0;		   // コンボカウント
		float timer = 0.0f;		   // タイマー
		float comboTimer = 0.0f;   // コンボタイマー
	};
	State state_{};

	// 攻撃中フラグ
	bool isAttacking_ = false;

	// コンボ可能フラグ
	bool canCombo_ = false;

	// 攻撃データ
	AttackData attackData_{};

	// 攻撃開始時の前方移動の強さ
	float moveForwardStrength_ = 0.5f; 

	// マウスの方向を向くサイの補間速度
	float rotationLerpSpeed_ = 55.0f;

private:
	/// <summary>
	/// 攻撃を開始
	/// </summary>
	/// <param name="attackID">開始する攻撃のID</param>
	/// <param name="weapon">使用する武器</param>
	bool StartAttack(int attackID, PlayerWeapon* weapon);

	/// <summary>
	/// コンボ攻撃を試行
	/// </summary>
	/// <param name="weapon">使用する武器</param>
	bool TryCombo(PlayerWeapon* weapon);

	/// <summary>
	/// 攻撃をキャンセル
	/// </summary>
	void CancelAttack();

	/// <summary>
	/// タイマーの更新
	/// </summary>
	void UpdateTimers(const float deltaTime);

	/// <summary>
	/// 武器に攻撃軌道を設定
	/// </summary>
	/// <param name="data"></param>
	/// <param name="weapon"></param>
	void ApplyAttackToWeapon(const AttackData& data, PlayerWeapon* weapon);

	/// <summary>
	/// 攻撃開始時に前方に移動します。
	/// </summary>
	void moveForwardOnAttackStart();

	/// <summary>
	/// Key入力の時にマウスの方向に向くようにする。
	/// </summary>
	void FaceMouseDirection();
};