/// ===Include=== ///
#include "Base/PlayerState.h"
// C++
#include <array>
#include <string>

/// ===前方宣言=== ///
class PlayerWeapon;

///=====================================================/// 
/// AttackState
///=====================================================///
class AttackState : public PlayerState {
public:

	AttackState();
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
	// 攻撃アニメーションの名前
	std::array<const char*, 3> attackAnimationNames_;

	// 入力を受け付ける進捗
	float comboAcceptNormalizeTime_ = 0.8f;

	// アニメーション終了後のコンボ受付時間
	float comboAcceptExtraTime_ = 0.2f;

	// 現在の攻撃
	int attackIndex_ = 0;

	// 攻撃中フラグ
	bool isAttacking_ = false;

	// コンボ可能フラグ
	bool canCombo_ = false;
	
	//アニメーション終了後のコンボ受付タイマー
	float comboAcceptTimer_ = 0.0f;

	// 攻撃開始時の前方移動の強さ
	float moveForwardStrength_ = 0.5f; 

	// マウスの方向を向くサイの補間速度
	float rotationLerpSpeed_ = 55.0f;

private:
	/// <summary>
	/// 攻撃を開始
	/// </summary>
	/// <param name="attackIndex">開始する攻撃のIndex</param>
	void StartAttack(int attackIndex);

	/// <summary>
	/// 攻撃をキャンセル
	/// </summary>
	void CancelAttack();

	/// <summary>
	/// 攻撃開始時に前方に移動します。
	/// </summary>
	void moveForwardOnAttackStart();

	/// <summary>
	/// Key入力の時にマウスの方向に向くようにする。
	/// </summary>
	void FaceMouseDirection();
};