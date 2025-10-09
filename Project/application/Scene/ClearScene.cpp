#include "ClearScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
ClearScene::~ClearScene() {
	// ISceneのデストラクタ
	IScene::~IScene();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void ClearScene::Initialize() {
	// ISceneの初期化(デフォルトカメラとカメラマネージャ)
	IScene::Initialize();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void ClearScene::Update() {
	/// ===デバック用ImGui=== ///
#ifdef USE_IMGUI
	ImGui::Begin("ClearScene");
	ImGui::End();
#endif // USE_IMGUI

	/// ===ISceneの更新=== ///
	IScene::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void ClearScene::Draw() {
#pragma region 背景スプライト描画
#pragma endregion

#pragma region モデル描画
	/// ===ISceneの描画=== ///
	IScene::Draw();
#pragma endregion

#pragma region 前景スプライト描画
#pragma endregion
}
