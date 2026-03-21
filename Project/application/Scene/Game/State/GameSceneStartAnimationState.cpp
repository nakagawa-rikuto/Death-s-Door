#include "GameSceneStartAnimationState.h"
// GameScene
#include "application/Scene/Game/GameScene.h"
// Service
#include "Service/Camera.h"
// State
#include "GameSceneInGameState.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
GameSceneStartAnimationState::~GameSceneStartAnimationState() {
	startAnimation_.reset();
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void GameSceneStartAnimationState::Enter(GameScene* gameScene) {
	// ゲームシーンのポインタを保存
	gameScene_ = gameScene;

	/// ===StartAnimationの初期化=== ///
	startAnimation_ = std::make_unique<StartAnimation>();
	startAnimation_->Initialize(gameScene_->GetPlayer(), gameScene_->GetCamera());
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void GameSceneStartAnimationState::Update() {
	// StartAnimationの更新
	startAnimation_->Update();

	// EnemyManagerの更新
	gameScene_->GetEnemyManager()->UpdateAnimation();

	// Playerの更新
	gameScene_->GetPlayer()->UpdateAnimation();

	if (startAnimation_->IsCompleted()) {
		// カメラターゲットをPlayerに設定
		gameScene_->GetPlayer()->SetCameraTargetPlayer();
		gameScene_->ChangState(std::make_unique<GameSceneInGameState>());
	}
}

///-------------------------------------------/// 
/// 描画処理
///-------------------------------------------///
void GameSceneStartAnimationState::Draw() {
	// StartAnimationの描画
	startAnimation_->Draw();
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void GameSceneStartAnimationState::Finalize() {
	startAnimation_.reset();
	GameSceneFadeState::Finalize();
}
