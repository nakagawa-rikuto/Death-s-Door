#include "GameSceneInGameState.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
// GameScene
#include "application/Scene/Game/GameScene.h"
// State
#include "GameSceneGameOverAnimationState.h"
#include "GameSceneGameClearState.h"
#include "GameSceneExitState.h"
// Service
#include "Service/Input.h"
#include "Service/DeltaTime.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
GameSceneInGameState::~GameSceneInGameState() {
	optionUI_.reset();
	ui_.reset();
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void GameSceneInGameState::Enter(GameScene* gameScene) {
	// ゲームシーンのポインタを保存
	gameScene_ = gameScene;

	/// ===GameSceneUI=== ///
	ui_ = std::make_unique<GameSceneUI>();
	ui_->Initialize(gameScene_->GetPlayer(), gameScene_->GetEnemy());
	ui_->Update();

	/// ===OptionUI=== ///
	optionUI_ = std::make_unique<OptionUI>();
	optionUI_->Initialize();
	isOptionActive_ = false;

	gameScene_->GetCamera()->SetFollowSpeed(1.0f);
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void GameSceneInGameState::Update() {

	/// ===OptionUI=== ///
	if (Service::Input::TriggerButton(0, ControllerButtonType::Start) || Service::Input::TriggerKey(DIK_ESCAPE)) {
		if (isOptionActive_) {
			Service::DeltaTime::SetDeltaTime(1.0f / 60.0f);
			isOptionActive_ = false;
		} else {
			Service::DeltaTime::SetDeltaTime(0.0f);
			isOptionActive_ = true;
		}
	}
	// OptionUIの更新
	optionUI_->GameUpdate(isOptionActive_);

	/// ===Gameの更新処理=== ///
	if (!isOptionActive_) {
		// EnemyManagerの更新
		gameScene_->GetEnemy()->Update();
		// Playerの更新
		gameScene_->GetPlayer()->Update();
		// GameSceneUIの更新
		ui_->Update();
	}

	/// ===Stateの移動=== ///
	// Playerが死亡しているか確認
	if (gameScene_->GetPlayer()->GetIsDead()) {
		// ゲームオーバーアニメーション状態に変更
		gameScene_->ChangState(std::make_unique<GameSceneGameOverAnimationState>());
	} else if (gameScene_->GetEnemy()->GetIsDead()) {
		// ゲームクリアアニメーション状態に変更
		gameScene_->ChangState(std::make_unique<GameSceneGameClearState>());
	} else if (optionUI_->GetReturnToTitle()) {
		Service::DeltaTime::SetDeltaTime(1.0f / 60.0f);
		// Exit状態に変更
		gameScene_->ChangState(std::make_unique<GameSceneExitState>());
	}
}

///-------------------------------------------/// 
/// 描画処理
///-------------------------------------------///
void GameSceneInGameState::Draw() {}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void GameSceneInGameState::Finalize() {
	ui_.reset();
	optionUI_.reset();
	GameSceneFadeState::Finalize();
}
