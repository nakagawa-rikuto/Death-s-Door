#pragma once
/// ===前方宣言=== ///
class Player;
class Camera;

///=====================================================/// 
/// プレイヤーのベースステート
///=====================================================///
class PlayerState {
public:
	virtual ~PlayerState() = default;

	// 状態に入ったときに呼ばれる
	virtual void Enter(Player* player, Camera* camera) = 0;

	// 状態の更新処理
	virtual void Update(Player* player, Camera* camera) = 0;

	// 終了処理
	virtual void Finalize();

protected:

	Player* player_ = nullptr; // Player
	Camera* camera_ = nullptr; // Camera
};
