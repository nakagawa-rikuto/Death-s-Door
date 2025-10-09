#pragma once
/// ===Include=== ///
#include "Engine/Scene/IScene.h"

///=====================================================/// 
/// クリアシーン
///=====================================================///
class ClearScene : public IScene {
public:/// ===メンバ関数=== ///

	ClearScene() = default;
	~ClearScene();

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;

private:/// ===メンバ変数=== ///
	/// <summary>
	/// シーン用
	/// </summary>

};

