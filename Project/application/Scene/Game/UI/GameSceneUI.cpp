#include "GameSceneUI.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include "Service/GraphicsResourceGetter.h"

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void GameSceneUI::SetPlayer(Player* player) {
	// PlayerUI
	playerUI_->SetPlayer(player);
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void GameSceneUI::Initialize() {
	// PlayerUIの生成と初期化
	playerUI_ = std::make_unique<PlayerUI>();
	playerUI_->Initialize();

	/// ===ウィンドウサイズの取得=== ///
	Vector2 windowSize = {
		static_cast<float>(Service::GraphicsResourceGetter::GetWindowWidth()),
		static_cast<float>(Service::GraphicsResourceGetter::GetWindowHeight())
	};

	/// ===OptionSprite=== ///
	optionMenuSprite_ = std::make_unique<Object2d>();
	optionMenuSprite_->Initialize("menuButton");
	optionMenuSprite_->SetPosition({ windowSize.x - 100.0f, 100.0f});
	optionMenuSprite_->SetSize({ 100.0f, 100.0f });
	optionMenuSprite_->SetAnchorPoint({ 0.5f, 0.5f });
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void GameSceneUI::Update() {
	// PlayerUIの更新
	playerUI_->Update();
}