#include "TitleScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
// Service
#include "Service/Camera.h"
#include "Service/PostEffect.h"
#include "Service/Particle.h"
// Math
#include "Math/sMath.h"
#include "Math/EasingMath.h"
#ifdef USE_IMGUI
// Service
#include "Service/Input.h"
#endif // USE_IMGUI


///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
TitleScene::~TitleScene() {
	// TitleUIのリセット
	titleUI_.reset();
	lines_.clear();
	// Animationのリセット
	animation_.reset();
	// Particleのリセット
	particle_->Stop();
	particle_ = nullptr;
	// Cameraの解放
	Service::Camera::Remove("Title");
	camera_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void TitleScene::Initialize() {
	// ISceneの初期化(デフォルトカメラとカメラマネージャ)
	IScene::Initialize();

	/// ===Particle=== ///
	Service::Particle::LoadParticleDefinition("Title.json");
	particle_ = Service::Particle::Emit("Title", { 0.0f, 0.0f, 0.0f });

	/// ===Line=== ///
	for (uint32_t i = 0; i < 2; ++i) {
		auto line = std::make_unique<Line>();
		lines_.push_back(std::move(line));
	}

	/// ===Camera=== ///
	camera_ = std::make_unique<MiiEngine::NormalCamera>();
	camera_->Initialize();
	camera_->SetTranslate({ 0.0f, 0.0f, 0.0f });
	// カメラの設定
	Service::Camera::AddCamera("Title", camera_.get());
	Service::Camera::SetActiveCamera("Title");

	/// ===TitleUI=== ///
	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize();

	/// ===Animation=== ///
	animation_ = std::make_unique<TitleSceneAnimation>();
	animation_->Initialize();

	/// ===Fadeの設定=== ///
	isStartFadeOut_ = false;
	currentFade_ = FadeState::FadeIn;
	float fadeDuration = 0.8f;
	sceneManager_->StartFadeIn(TransitionType::BlackOut, fadeDuration);

	/// ===PostEffectの設定=== ///
	Service::PostEffect::SetOffScreenType(MiiEngine::OffScreenType::Vignette);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void TitleScene::Update() {
	/// ===デバック用ImGui=== ///
#ifdef USE_IMGUI
	ImGui::Begin("TitleScene");
	ImGui::Text("Fade State: %d", static_cast<int>(currentFade_));
	ImGui::End();

	// Camera
	camera_->ImGuiUpdate();

#endif // USE_IMGUI

	/// ===オブジェクトの回転・色アニメーション=== ///
	animation_->UpdateObjectsAnimation();

	/// ===フェーズ別更新=== ///
	switch (currentFade_) {
	case TitleScene::FadeState::FadeIn:
		UpdateFadeIn();
		break;
	case TitleScene::FadeState::Selecting:
		titleUI_->Update();
		if (titleUI_->IsGameStart()) {
			currentFade_ = FadeState::FadeOut;
			Service::PostEffect::SetOffScreenType(MiiEngine::OffScreenType::RadiusBlur);
		}
		break;
	case TitleScene::FadeState::FadeOut:
		UpdateFadeOut();
		break;
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void TitleScene::Draw() {

	// Lineの描画
	const auto& rotations = animation_->GetRotations();
	for (size_t i = 0; i < lines_.size() && i < rotations.size(); ++i) {
		lines_[i]->DrawTorus({ 0.0f, 0.0f, 50.0f }, rotations[i], animation_->GetSize(), animation_->GetColor());
	}
}

///-------------------------------------------/// 
/// フェードインの更新
///-------------------------------------------///
void TitleScene::UpdateFadeIn() {

	/// ===Fadeの変更=== ///
	if (sceneManager_->GetTransitionFinished()) {
		currentFade_ = FadeState::Selecting;
	}
}

///-------------------------------------------/// 
/// フェードアウトの更新
///-------------------------------------------///
void TitleScene::UpdateFadeOut() {
	// ラジアスブラーの更新
	animation_->UpdateRadiusBlurAnimation();

	// アニメーション終了後にフェードアウトへ移行
	if (animation_->GetStartFadeOut() && !isStartFadeOut_) {
		isStartFadeOut_ = true;
		sceneManager_->StartFadeOut(TransitionType::ShatterGlass, 1.0f);
	}

	/// ===シーンの切り替え=== ///
	if (sceneManager_->GetTransitionFinished()) {
		// ゲームシーンへ遷移
		sceneManager_->ChangeScene(MiiEngine::SceneType::Game);
	}
}