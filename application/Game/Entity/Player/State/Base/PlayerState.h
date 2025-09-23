#pragma once
/// ===前方宣言=== ///
class Player;
class GameCamera;

///=====================================================/// 
/// プレイヤーのベースステート
///=====================================================///
class PlayerState {
public:
	virtual ~PlayerState() = default;

	// 状態に入ったときに呼ばれる
	virtual void Enter(Player* player, GameCamera* camera) = 0;

	// 状態の更新処理
	virtual void Update(Player* player, GameCamera* camera) = 0;

	// 終了処理
	virtual void Finalize();

protected:

	Player* player_ = nullptr; // Player
	GameCamera* camera_ = nullptr; // Camera
};
