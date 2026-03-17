#include "SelectScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
SelectScene::~SelectScene() {
	// ISceneのデストラクタ
	IScene::~IScene();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void SelectScene::Initialize() {
	// ISceneの初期化(デフォルトカメラとカメラマネージャ)
	IScene::Initialize();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void SelectScene::Update() {
	/// ===デバック用ImGui=== ///
#ifdef USE_IMGUI
	ImGui::Begin("SelectScene");
	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void SelectScene::Draw() {}
