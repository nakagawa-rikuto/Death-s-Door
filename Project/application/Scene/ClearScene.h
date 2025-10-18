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

