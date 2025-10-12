#pragma once
/// ===Include=== ///
#include "Base/PlayerState.h"
// Math
#include "Math/Vector3.h"

///=====================================================/// 
/// AttackState
///=====================================================///
class AttackState : public PlayerState {
public:

	~AttackState() override = default;

	// 状態に入ったときに呼ばれる
	void Enter(Player* player, GameCamera* camera) override;
	// 状態の更新処理
	void Update(Player* player, GameCamera* camera) override;
	// 終了処理はPlayerStateのFinalizeを呼び出す
	void Finalize() override;

private:

	/// ===攻撃タイプ=== ///
	enum class AttackType {
		kCombo1,	// コンボ1段目（左から右へ）
		kCombo2,	// コンボ2段目（右から左へ）
	};

	/// ===攻撃情報=== ///
	struct AttackInfo {
		AttackType currentAttack = AttackType::kCombo1; // 現在の攻撃タイプ

		float activTime = 0.0f;			// 攻撃のアクティブ時間
		float comboWindowTime = 0.5f;	// コンボ受付時間
		float comboTimer = 0.0f;		// コンボ受付タイマー

		bool isAttacking = false;		// 攻撃中フラグ
		bool canCombo = false;			// コンボ可能フラグ
		bool nextComboRequest = false;	// 次のコンボ入力フラグ

		// 各攻撃の接続時間
		float combo1Duration = 0.4f;	// コンボ1段目の持続時間
		float combo2Duration = 0.6f;	// コンボ2段目の持続時間

		// クールタイム
		float coolTime = 0.3f;			// 攻撃後のクールタイム
	};
	AttackInfo attackInfo_;

private:
	// 攻撃の初期化
	void InitializeAttack(AttackType type);
	// コンボ入力のチェック
	void CheckComboInput();
	// チャージ攻撃入力のチェック
	void CheckChargeAttackInput();
	// 次の攻撃へ遷移
	void TransitionToNextCombo();
};