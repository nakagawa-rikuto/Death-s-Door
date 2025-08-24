#include "ChargeState.h"
// Player, Camera
#include "application/Game/Entity/Player/Player.h"
#include "application/Game/Camera/Camera.h"
// State
#include "RootState.h"

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
ChargeState::ChargeState(const Vector3& direction) {info_.direction = direction;}

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void ChargeState::Enter(Player* player, Camera* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	// 突進情報の初期化
	info_.acceleration = 0.2f;
	player_->SetTimer(actionType::kCharge, info_.activeTime);
	player_->SetStateFlag(actionType::kCharge, true);
	player_->SetpreparationFlag(actionType::kCharge, false);
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void ChargeState::Update(Player * player, Camera * camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	// 無敵時間の変更
	player_->SetInvicibleTime(info_.invincibleTime);
	// 加速度の減少
	info_.acceleration -= player_->GetDeltaTime() * info_.activeTime;
	// 速度の設定
	info_.speed = 0.4f * info_.acceleration;

	// Velocityに反映
	player_->SetVelocityX(info_.direction.x * info_.speed);
	player_->SetVeloctiyZ(info_.direction.z * info_.speed);

	/// ===タイマーが時間を超えたら=== ///
	if (player_->GetTimer(actionType::kCharge) <= 0.0f) {
		player_->SetTimer(actionType::kCharge, info_.cooltime);// クールタイムのリセット
		player_->SetStateFlag(actionType::kCharge, false);
		// Stateの変更
		player_->ChangState(std::make_unique<RootState>());
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void ChargeState::Finalize() {
	PlayerState::Finalize();
}
