#include "DeathState.h"

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void DeathState::Enter(Player* player, MiiEngine::CameraCommon* camera) {
	player_ = player;
	camera_ = camera;
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void DeathState::Update() {
	// 減速処理
	PlayerState::ApplyDeceleration(decelerationRate_);
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void DeathState::Finalize() {
	PlayerState::Finalize();
}
