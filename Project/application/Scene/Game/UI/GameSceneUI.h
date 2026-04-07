#pragma once
/// ===Include=== ///
#include "application/Game/UI/Game/PlayerUI.h"
#include "application/Game/UI/Game/BossUI.h"

/// ===前方宣言=== ///
class Player;
class BossEnemy;

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
	/// <param name="player">初期化するプレイヤーへのポインタ。</param>
	/// <param name="boss">初期化するボスエネミーへのポインタ。</param>
	void Initialize(Player* player, BossEnemy* boss);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

private:

	/// ===Class=== ///
	// PlayerUI
	std::unique_ptr<PlayerUI> playerUI_;
	// BossUI
	std::unique_ptr<BossUI> bossUI_;

	/// ===Object2d=== ///
	std::unique_ptr<Object2d> optionMenuSprite_;
};

