#pragma once
/// ===Include=== ///
// Base
#include "Base/GameSceneFadeState.h"
// Phase
#include "application/Game/Phase/Mob/MobPhase.h"
// GameUI
#include "application/Scene/Game/UI/GameSceneUI.h"
#include "application/Game/UI/Option/OptionUI.h"
// C++
#include <memory>

///=====================================================/// 
/// ゲームシーンフェードイン状態
///=====================================================///
class GameSceneInGameState : public GameSceneFadeState {
public:

	~GameSceneInGameState() override;

	/// <summary>
	/// ゲームシーンに入る際に呼び出されるオーバーライドメソッドです。
	/// </summary>
	/// <param name="gameScene">入るゲームシーンへのポインタ。</param>
	void Enter(GameScene* gameScene) override;

	/// <summary>
	/// フレームごとに呼び出される更新関数をオーバーライドします。
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理を実行します。
	/// </summary>
	void Draw() override;

	/// <summary>
	/// オブジェクトを終了処理します。
	/// </summary>
	void Finalize() override;

private:
	
	/// ===MobPhase=== ///
	std::unique_ptr<MobPhase> mobPhase_;

	/// ===GameSceneUI=== ///
	std::unique_ptr<GameSceneUI> ui_;

	/// ===OptionUI=== ///
	std::unique_ptr<OptionUI> optionUI_;
	// オプションがアクティブかどうか
	bool isOptionActive_ = false;
};
