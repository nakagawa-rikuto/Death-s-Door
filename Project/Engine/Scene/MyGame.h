#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Scene/Framework.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"

///=====================================================/// 
/// ゲーム全体
///=====================================================///
class MyGame : public Framework {
public:

	MyGame();
	~MyGame();

	// 初期化
	void Initialize(const wchar_t* title) override;
	// 終了
	void Finalize() override;
	// 毎フレーム更新
	void Update() override;
	// 描画
	void Draw() override;

private:/// ====== ///
	// ゲーム終了フラグ
	bool endRequst_ = false;

	/// ===シーン=== ///
	// シーンマネージャ
	std::unique_ptr<SceneManager> sceneManager_;
private:
	/// ===Load=== ///
	void LoadAudio();
	void LoadTexture();
	void LoadModel();
	void LoadAnimation();
};

