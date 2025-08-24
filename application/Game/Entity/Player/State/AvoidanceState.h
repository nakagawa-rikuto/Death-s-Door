#pragma once
/// ===Include=== ///
#include "Base/PlayerState.h"
// Math
#include "Math/Vector3.h"

///=====================================================/// 
/// AvoidanceState
///=====================================================///
class AvoidanceState : public PlayerState {
public:
	AvoidanceState(const Vector3& direction);
	~AvoidanceState() override = default;

	// 状態に入ったときに呼ばれる
	void Enter(Player* player, Camera* camera) override;
	// 状態の更新処理
	void Update(Player* player, Camera* camera) override;
	// 終了処理はPlayerStateのFinalizeを呼び出す
	void Finalize() override;

private:

	/// ===回避情報=== ///
	struct AvoidanceInfo {
		float speed = 0.0f;			  // 回避時の移動速度
		float activeTime = 0.3f;      // 回避の有効時間
		float cooltime = 1.0f;		  // 回避のクールタイム
		float invincibleTime = 0.3f;  // 回避時の無敵時間
		float acceleration = 0.0f;    // 回避の加速度
		Vector3 direction = { 0.0f, 0.0f, 0.0f };
	};
	AvoidanceInfo info_;
};

