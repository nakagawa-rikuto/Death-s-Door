#pragma once
/// ===Include=== ///
#include "application/Game/UI/Game/PlayerUI.h"

/// ===前方宣言=== ///
class Player;

///=====================================================/// 
/// GameSceneUI
///=====================================================///
class GameSceneUI {
public:

	GameSceneUI() = default;
	~GameSceneUI() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

public: /// ===Setter=== ///
	// Playerのセット
	void SetPlayer(Player* player);

private:

	/// ===Class=== ///
	// PlayerUI
	std::unique_ptr<PlayerUI> playerUI_;

	/// ===Object2d=== ///
	std::unique_ptr<Object2d> optionMenuSprite_;
};

