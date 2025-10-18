#pragma once
/// ===Include=== ///
// IScene
#include "Engine/Scene/IScene.h"

///=====================================================/// 
/// ゲームオーバーシーン
///=====================================================///
class GameOverScene : public IScene {
public:/// ===メンバ関数=== ///

	GameOverScene() = default;
	~GameOverScene();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

private:/// ===メンバ変数=== ///
	/// <summary>
	/// シーン用
	/// </summary>
};

