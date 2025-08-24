#pragma once
/// ===Include=== ///
#include "Base/PlayerState.h"
// Math
#include "Math/Vector3.h"

///=====================================================/// 
/// MoveState
///=====================================================///
class MoveState : public PlayerState{
public:

	~MoveState() override = default;

	// 状態に入ったときに呼ばれる
	void Enter(Player* player, Camera* camera) override;
	// 状態の更新処理
	void Update(Player* player, Camera* camera) override;
	// 終了処理はPlayerStateのFinalizeを呼び出す
	void Finalize() override;

private:

	/// ===移動情報=== ///
	struct MoveInfo {
		float speed = 0.5f;
		Vector3 direction = { 0.0f, 0.0f, 0.0f };
	};
	MoveInfo info_;
};

