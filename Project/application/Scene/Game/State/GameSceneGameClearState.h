#pragma once
/// ===Include=== ///
// Base
#include "Base/GameSceneFadeState.h"
// GameClearAnimation
#include "application/Game/Animation/GameClearAnimation.h"
// C++
#include <memory>

///=====================================================/// 
/// ゲームシーンフェードイン状態
///=====================================================///
class GameSceneGameClearState : public GameSceneFadeState {
public:

	~GameSceneGameClearState() override;

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
	// GameClearAnimation
	std::unique_ptr<GameClearAnimation> gameClearAnimation_;
};


