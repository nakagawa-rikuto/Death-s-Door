#include "BossAttackState.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include "MoveBossState.h"

///-------------------------------------------/// 
/// 開始時に呼び出す
///-------------------------------------------///
void BossAttackState::Enter(BossEnemy* enemy) {
	boss_ = enemy;
	// velocityをリセット
	boss_->SetVelocity({ 0.0f, 0.0f, 0.0f });

	// -----------------------------------------------
	// Enter時点で即座に攻撃を選択・開始する。
	// MoveBossState側で IsAnyAttackAvailable() が true に
	// なってから遷移してくるので、ここでは必ず選択できる。
	// -----------------------------------------------
	//const float dist = CalcDistToPlayer();
	//const Vector3& bossPos = boss_->GetTransform().translate;
	//const Vector3& playerPos = boss_->GetPlayer()->GetTransform().translate;
	//const Quaternion& rot = boss_->GetTransform().rotate;
	//boss_->GetAttackComponent().SelectAndStart(dist, bossPos, playerPos, rot);
}

///-------------------------------------------/// 
/// 更新時に呼び出す
///-------------------------------------------///
void BossAttackState::Update() {
	//// コンテキストの準備
	//BossAttackManager::UpdateContext context{
	//	.bossRotation = boss_->GetTransform().rotate,
	//	.bossPosition = boss_->GetTransform().translate,
	//	.deltaTime = boss_->GetDeltaTime(),
	//};
	//// AttackManagerを更新
	//BossAttackManager::UpdateResult result = boss_->GetAttackComponent().Update(context);

	//// 攻撃中はボスへ結果を反映して終了
	//if (result.isAttacking) {
	//	// 回転を反映
	//	boss_->SetRotate(result.modelRotation);

	//	// 武器のオフセットを反映
	//	boss_->GetWeapon().SetTranslate(result.weaponLocalOffset);

	//	// 踏み込みなどの位置デルタを反映
	//	// （Y方向は物理演算に委ねるため上書きしない）
	//	if (result.modelPositionDelta.x != 0.0f ||
	//		result.modelPositionDelta.z != 0.0f) {
	//		Vector3 vel = result.modelPositionDelta / boss_->GetDeltaTime();
	//		vel.y = boss_->GetVelocity().y;
	//		boss_->SetVelocity(vel);
	//	}
	//	return;
	//}

	//// -----------------------------------------------
	//// 攻撃が完了したフレーム（justFinished == true）の処理。
	//// 次の攻撃をすぐ打てるか判定し、
	////   → 打てる : AttackStateのまま次の攻撃を開始（Stateを抜けない）
	////   → 打てない: MoveBossStateへ戻る
	//// -----------------------------------------------
	//if (result.justFinished) {
	//	// 攻撃完了後に velocityをリセット（踏み込み速度が残らないように）
	//	boss_->SetVelocity({ 0.0f, boss_->GetVelocity().y, 0.0f });

	//	const float dist = CalcDistToPlayer();

	//	if (boss_->GetAttackComponent().IsAnyAttackAvailable(dist)) {
	//		// 次の攻撃を即座に開始（ChangeState不要・コスト0）
	//		const Vector3& bossPos = boss_->GetTransform().translate;
	//		const Vector3& playerPos = boss_->GetPlayer()->GetTransform().translate;
	//		const Quaternion& rot = boss_->GetTransform().rotate;

	//		boss_->GetAttackComponent().SelectAndStart(dist, bossPos, playerPos, rot);
	//	} else {
	//		/// ===Stateの変更=== ///
	//		boss_->ChangeState(std::make_unique<MoveBossState>());
	//	}
	//}
}

///-------------------------------------------/// 
/// 終了時に呼び出す
///-------------------------------------------///
void BossAttackState::Finalize() {
	// Componentの状態を確実にリセットしておく
	//boss_->GetAttackComponent().ForceReset();
	// 終了処理
	BossState::Finalize();
}

///-------------------------------------------/// 
/// プレイヤーとボスの距離を計算して返す。
///-------------------------------------------///
float BossAttackState::CalcDistToPlayer() const {
	const Vector3 diff =
		boss_->GetPlayer()->GetTransform().translate -
		boss_->GetTransform().translate;
	return Length(diff);
}


