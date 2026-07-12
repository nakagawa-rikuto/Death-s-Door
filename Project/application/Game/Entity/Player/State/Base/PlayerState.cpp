#include "PlayerState.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include "Service/Input.h"

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void PlayerState::Finalize() {
	player_ = nullptr;
	camera_ = nullptr;
}

///-------------------------------------------/// 
/// 減速処理
///-------------------------------------------///
void PlayerState::ApplyDeceleration(const float& deceleration) {
	// 現在の速度を取得
	Vector3 velocity = player_->GetVelocity();

	// Velocityが0ではないなら徐々に0にする
	if (velocity.x != 0.0f) { // X軸の速度に対して減速を適用
		velocity.x *= deceleration;
		// 速度が小さくなりすぎたら0にする
		if (std::abs(velocity.x) < 0.01f) {
			velocity.x = 0.0f;
		}
	}

	if (velocity.z != 0.0f) { // Z軸の速度に対して減速を適用
		velocity.z *= deceleration;
		// 速度が小さくなりすぎたら0にする
		if (std::abs(velocity.z) < 0.01f) {
			velocity.z = 0.0f;
		}
	}

	// 減速後の速度をプレイヤーに設定
	player_->SetVelocity(velocity);
}
