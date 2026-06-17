#include "DodgeState.h"
// Player, Camera
#include "application/Game/Entity/Player/Player.h"
// State
#include "RootState.h"

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
DodgeState::DodgeState(const Vector3& direction) {
	state_.direction = direction;
}

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void DodgeState::Enter(Player * player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;

	// 回避行動のアクティブフラグを設定
	player_->SetActiveDodgeFlag(true);

	// 回避処理を開始
	StartAvoidance();
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void DodgeState::Update() {

	/// ===タイマ-の更新=== ///
	UpdateTimer();

	/// ===回避処理=== ///
	UpdateAvoidance();

	/// ===タイマーが時間を超えたら=== ///
	if (state_.timer <= 0.0f) {
		// Stateの変更
		player_->ChangState(std::make_unique<RootState>());
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void DodgeState::Finalize() {
	// 回避行動のクールタイムを設定
	player_->SetDodgeCoolDown();
	// 回避行動のアクティブフラグをリセット
	player_->SetActiveDodgeFlag(false);

	PlayerState::Finalize();
}

///-------------------------------------------/// 
/// 回避開始処理
///-------------------------------------------///
void DodgeState::StartAvoidance() {
	// 状態の初期化
	state_.acceleration = 0.1f;
	state_.timer = player_->GetParameters().dodge.activeTime;
}

///-------------------------------------------/// 
/// タイマーの更新処理
///-------------------------------------------///
void DodgeState::UpdateTimer() {
	if (state_.timer > 0.0f) {
		state_.timer -= player_->GetDeltaTime();
	}
}

///-------------------------------------------/// 
/// 回避処理の更新
///-------------------------------------------///
void DodgeState::UpdateAvoidance() {
	// 加速度の減少
	state_.acceleration -= player_->GetDeltaTime() * player_->GetParameters().dodge.activeTime;
	// 速度の設定
	float speed = player_->GetParameters().dodge.speed * state_.acceleration;

	// Velocityに反映
	state_.velocity.x += state_.direction.x * speed;
	state_.velocity.z += state_.direction.z * speed;

	// プレイヤーの速度に反映
	player_->SetVelocity(state_.velocity);
}