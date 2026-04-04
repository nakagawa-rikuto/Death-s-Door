#include "HitReactionState.h"
// Player
#include <application/Game/Entity/Player/Player.h>
// GroundOcean
#include <application/Game/Object/GameGround/GroundOcean.h>
// State
#include "RootState.h"
// Collider
#include "Engine/Collider/ColliderConcept.h"
// Component
#include "application/Game/Entity/Player/Component/PlayerHitReactionComponent.h"

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
HitReactionState::HitReactionState(const Vector3& direction) {
	knockbackDirection_ = direction;
}

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void HitReactionState::Enter(Player* player, MiiEngine::CameraCommon* camera) {
	// 引数の取得
	player_ = player;
	camera_ = camera;
	
	// 更新結果の取得
	auto result = player_->GetHitReactionComponent()->Start(knockbackDirection_);
	// 結果の反映
	player_->SetVelocity(result.velocity);

	// 波紋を出す
	player_->GetGroundOcean()->AddRipple(player_->GetTransform().translate, 0.5f, 0.05f);
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void HitReactionState::Update(Player * player, MiiEngine::CameraCommon * camera) {
	// 引数の更新
	player_ = player;
	camera_ = camera;

	// 減速処理
	player_->ApplyDeceleration(player_->GetMoveComponent()->GetConfig().deceleration);
	
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
