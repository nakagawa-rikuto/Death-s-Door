#include "AttackState.h"
// Service
#include "Engine/System/Service/InputService.h"
// Player
#include "application/Game/Entity/Player/Player.h"
#include "application/Game/Entity/Player/Weapon/PlayerWeapon.h"
// State
#include "RootState.h"
#include "ChargeState.h"

///-------------------------------------------/// 
/// 状態に入ったときに呼ばれる
///-------------------------------------------///
void AttackState::Enter(Player* player, GameCamera* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	// 攻撃フラグを立てる
	player_->SetStateFlag(actionType::kAttack, true);
	player_->SetpreparationFlag(actionType::kAttack, false);

	// 最初の攻撃を初期化
	InitializeAttack(AttackType::kCombo1);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void AttackState::Update(Player* player, GameCamera* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	// アクティブタイマーを進める
	attackInfo_.activTime -= player_->GetDeltaTime();

	// コンボ受付タイマーを進める
	if (attackInfo_.canCombo) {
		attackInfo_.comboTimer -= player_->GetDeltaTime();

		// コンボ受付時間を過ぎたらコンボ受付終了
		if (attackInfo_.comboTimer <= 0.0f) {
			attackInfo_.canCombo = false;
		}
	}

	// コンボ入力チェック
	CheckComboInput();

	// チャージ攻撃の入力チェック
	CheckChargeAttackInput();

	// 攻撃中の処理
	if (attackInfo_.isAttacking) {
		// 攻撃時間が終了
		if (attackInfo_.activTime <= 0.0f) {
			attackInfo_.isAttacking = false;

			// 次のコンボ入力がされている場合
			if (attackInfo_.nextComboRequest && attackInfo_.canCombo) {
				TransitionToNextCombo();
			} else {
				// 攻撃終了、通常状態へ
				player_->SetTimer(actionType::kAttack, attackInfo_.coolTime);
				player_->SetStateFlag(actionType::kAttack, false);
				player_->ChangState(std::make_unique<RootState>());
			}
		}
	}
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void AttackState::Finalize() {
	PlayerState::Finalize();
}

///-------------------------------------------/// 
/// 攻撃の初期化
///-------------------------------------------///
void AttackState::InitializeAttack(AttackType type) {
	attackInfo_.currentAttack = type;
	attackInfo_.isAttacking = true;
	attackInfo_.canCombo = false;
	attackInfo_.nextComboRequest = false;

	// 武器の取得（nullチェック）
	PlayerWeapon* weapon = player_->GetWeapon();
	if (!weapon) return;

	// 攻撃タイプに応じて時間と軌道を設定
	switch (type) {
	case AttackState::AttackType::kCombo1:
		// コンボ1段目（左から右へ）
		attackInfo_.activTime = attackInfo_.combo1Duration;
		attackInfo_.canCombo = true;
		attackInfo_.comboTimer = attackInfo_.comboWindowTime;

		// 武器の軌道を設定（左から右へ）
		weapon->Attack(
			{ -2.0f, 1.0f, 1.0f },  // 開始位置：左側
			{ 2.0f, 1.0f, 1.0f },   // 終了位置：右側
			attackInfo_.combo1Duration
		);
		break;

	case AttackState::AttackType::kCombo2:
		// コンボ2段目（右から左へ）
		attackInfo_.activTime = attackInfo_.combo2Duration;
		attackInfo_.canCombo = false; // 2段目はコンボ終了

		// 武器の軌道を設定（右から左へ）
		weapon->Attack(
			{ 2.0f, 1.0f, 1.0f },   // 開始位置：右側
			{ -2.0f, 1.0f, 1.0f },  // 終了位置：左側
			attackInfo_.combo2Duration
		);
		break;
	}
}

///-------------------------------------------/// 
/// コンボ入力のチェック
///-------------------------------------------///
void AttackState::CheckComboInput() {
	// Xボタンのトリガー入力でコンボ継続
	if (InputService::TriggerButton(0, ControllerButtonType::X)) {
		// コンボ受付中なら次のコンボをリクエスト
		if (attackInfo_.canCombo) {
			attackInfo_.nextComboRequest = true;
		}
	}
}

///-------------------------------------------/// 
/// チャージ攻撃入力のチェック
///-------------------------------------------///
void AttackState::CheckChargeAttackInput() {
	// Yボタン長押しでチャージ攻撃へ移行
	if (InputService::PushButton(0, ControllerButtonType::Y)) {
		// チャージ攻撃状態へ遷移
		player_->SetStateFlag(actionType::kAttack, false);
		player_->ChangState(std::make_unique<ChargeState>());
	}
}

///-------------------------------------------/// 
/// 次のコンボへ遷移
///-------------------------------------------///
void AttackState::TransitionToNextCombo() {
	attackInfo_.nextComboRequest = false;

	// 現在の攻撃タイプに応じて次の攻撃へ
	switch (attackInfo_.currentAttack) {
	case AttackType::kCombo1:
		InitializeAttack(AttackType::kCombo2);
		break;
	case AttackType::kCombo2:
		// 2段目で終了（必要に応じて3段目を追加可能）
		break;
	}
}