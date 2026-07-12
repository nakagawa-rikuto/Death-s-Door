#include "SelectScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
// Service
#include "Service/PostEffect.h"

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

	// ポストエフェクトをクリア
	Service::PostEffect::ClearEffects();
	Service::PostEffect::AddEffect(MiiEngine::OffScreenType::CopyImage);

	// Oceanを生成
	oceanGround_ = std::make_unique<GroundOcean>();
	oceanGround_->Initialize();

	// カメラの位置を設定
	defaultCamera_->SetTranslate({ 0.0f, 50.0f, -100.0f });
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
