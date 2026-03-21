#pragma once
/// ===Include=== ///
// Base
#include "Base/GameSceneFadeState.h"
// StartAnimation
#include "application/Game/Animation/StartAnimation.h"
// C++
#include <memory>

///=====================================================/// 
/// ゲームシーンフェードイン状態
///=====================================================///
class GameSceneStartAnimationState : public GameSceneFadeState {
public:

	~GameSceneStartAnimationState() override;

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
	// StartAnimation
	std::unique_ptr<StartAnimation> startAnimation_;

};