#pragma once
/// ===Include=== ///
#include "Base/PlayerState.h"

///=====================================================/// 
/// プレイヤーの通常状態のステート
///=====================================================///
class RootState : public PlayerState {
public:

	~RootState() override = default;

	// 状態に入ったときに呼ばれる
	void Enter(Player* player, GameCamera* camera) override;
	// 状態の更新処理
	void Update(Player* player, GameCamera* camera) override;
	// 終了処理はPlayerStateのFinalizeを呼び出す
	void Finalize() override;

private:
	// 減速率
	const float deceleration_ = 0.75f;
};

