#include "SelectScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
SelectScene::~SelectScene() {
	// ISceneのデストラクタ
	IScene::~IScene();
	oceanGround_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void SelectScene::Initialize() {
	// ISceneの初期化(デフォルトカメラとカメラマネージャ)
	IScene::Initialize();

	oceanGround_ = std::make_unique<GroundOcean>();
	oceanGround_->Initialize();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void SelectScene::Update() {
	/// ===デバック用ImGui=== ///
#ifdef USE_IMGUI
	ImGui::Begin("SelectScene");

	defaultCamera_->ImGuiUpdate();
	defaultCamera_->DebugUpdate();

	oceanGround_->Information();
	oceanGround_->ShowImGui();
	ImGui::End();
#endif // USE_IMGUI

	oceanGround_->Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void SelectScene::Draw() {
	oceanGround_->Draw();
}
