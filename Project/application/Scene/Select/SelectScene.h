#pragma once
/// ===Include=== ///
// IScene
#include "Engine/Scene/IScene.h"

// OceanGround
#include <application/Game/Object/GameGround/GroundOcean.h>

///=====================================================/// 
/// セレクトシーン
///=====================================================///
class SelectScene : public MiiEngine::IScene {
public:/// ===メンバ関数=== ///

	SelectScene() = default;
	~SelectScene();

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
	
	std::unique_ptr<GroundOcean> oceanGround_;
};

