#include "AttackState.h"
// Service
#include "Engine/System/Service/InputService.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include "RootState.h"

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
void AttackState::Update(Player * player, GameCamera * camera) {
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

	// 攻撃タイプに応じて時間を設定
	switch (type) {
	case AttackState::AttackType::kCombo1:
		// コンボ1段目（右から左へ）
		attackInfo_.activTime = attackInfo_.combo1Duration;
		attackInfo_.canCombo = true;
		attackInfo_.comboTimer = attackInfo_.comboWindowTime;
		break;

	case AttackState::AttackType::kCombo2:
		// コンボ2段目（左から右へ）
		attackInfo_.activTime = attackInfo_.combo2Duration;
		attackInfo_.canCombo = false; // 2段目はコンボ終了
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
/// 次のコンボへ遷移
///-------------------------------------------///
void AttackState::TransitionToNextCombo() {
	attackInfo_.nextComboRequest = false;

	// 現在の攻撃タイプに応じて次の攻撃へ
	switch (attackInfo_.currentAttack){
	case AttackType::kCombo1:
		InitializeAttack(AttackType::kCombo2);
		break;
	case AttackType::kCombo2:
		break;
	}
}
