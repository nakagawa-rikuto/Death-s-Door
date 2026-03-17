#include "GameOverScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
GameOverScene::~GameOverScene() {
	// ISceneのデストラクタ
	IScene::~IScene();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void GameOverScene::Initialize() {
	// ISceneの初期化(デフォルトカメラとカメラマネージャ)
	IScene::Initialize();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void GameOverScene::Update() {
	/// ===デバック用ImGui=== ///
#ifdef USE_IMGUI
	ImGui::Begin("GameOverScene");
	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void GameOverScene::Draw() {}
