#include "BossState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void BossState::Finalize() {
	boss_ = nullptr;
}

///-------------------------------------------/// 
/// プレイヤーとボスの距離を計算して返す。
///-------------------------------------------///
float BossState::CalcDistToPlayer() const {
	const Vector3 diff = boss_->GetPlayer()->GetTransform().translate - boss_->GetTransform().translate;
	return Length(diff);
}
