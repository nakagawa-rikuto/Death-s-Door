#include "GameSceneUI.h"
// Service
#include "Service/GraphicsResourceGetter.h"

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void GameSceneUI::Initialize(Player* player, BossEnemy* boss) {
	
	/// ===ウィンドウサイズの取得=== ///
	Vector2 windowSize = {
		static_cast<float>(Service::GraphicsResourceGetter::GetWindowWidth()),
		static_cast<float>(Service::GraphicsResourceGetter::GetWindowHeight())
	};

	/// ===PlayerUI=== ///
	playerUI_ = std::make_unique<PlayerUI>();
	playerUI_->Initialize(player, windowSize);

	/// ===BossUi=== ///
	bossUI_ = std::make_unique<BossUI>();
	bossUI_->Initialize(boss, windowSize);

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
	/// ===UIの更新処理=== ///
	playerUI_->Update();
	bossUI_->Update();
}