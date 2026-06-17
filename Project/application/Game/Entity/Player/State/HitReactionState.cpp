#include "HitReactionState.h"
// Player
#include <application/Game/Entity/Player/Player.h>
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include "RootState.h"

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
HitReactionState::HitReactionState(const Vector3& direction) {
	state_.knockbackDirection = direction;
}

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void HitReactionState::Enter(Player* player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;
	
	// ヒットリアクションの開始
	StartHitReaction();

	// 波紋を出す
	player_->GetGroundOcean()->AddRipple(player_->GetTransform().translate, 0.5f, 0.05f);
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void HitReactionState::Update() {

	// 減速処理
	PlayerState::ApplyDeceleration(player_->GetParameters().move.deceleration);
	
	/// ===Stateの変更=== ///
	if (std::abs(player_->GetVelocity().x) <= 0.01f && std::abs(player_->GetVelocity().z) <= 0.01f) {
		// RootStateへ移行
		player_->ChangState(std::make_unique<RootState>());
	}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void HitReactionState::Finalize() {
	PlayerState::Finalize();
}

///-------------------------------------------/// 
/// ヒットリアクションを開始します
///-------------------------------------------///
void HitReactionState::StartHitReaction() {
	// Y軸のノックバックを無効化
	state_.knockbackDirection.y = 0.0f; 

	// XZ平面のノックバック方向を正規化
	if (state_.knockbackDirection.x != 0.0f || state_.knockbackDirection.z != 0.0f) {
		state_.knockbackDirection = Normalize(state_.knockbackDirection);
	}

	// ノックバックの速度を設定
	state_.velocity = state_.knockbackDirection * player_->GetParameters().hitReaction.knockbackSpeed;
	player_->SetVelocity(state_.velocity);
}
