#include "GameSceneGameOverAnimationState.h"
// GameScene
#include "application/Scene/Game/GameScene.h"
// Service
#include "Service/Camera.h"
// State
#include "GameSceneExitState.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
GameSceneGameOverAnimationState::~GameSceneGameOverAnimationState() {
	gameOverAnimation_.reset();
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void GameSceneGameOverAnimationState::Enter(GameScene* gameScene) {
	// ゲームシーンのポインタを保存
	gameScene_ = gameScene;

	/// ===GameOverAnimation=== ///
	gameOverAnimation_ = std::make_unique<GameOverAnimation>();
	gameOverAnimation_->Initialize(gameScene_->GetCamera());
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void GameSceneGameOverAnimationState::Update() {
	// GameOverAnimationの更新
	gameOverAnimation_->Update();

	// EnemyManagerの更新
	gameScene_->GetEnemyManager()->UpdateAnimation();
	// Playerの更新
	gameScene_->GetPlayer()->Update();

	// アニメーション完了を確認
	if (gameOverAnimation_->IsCompleted()) {
		//NOTE:ゲームオーバーの場合はタイトルに戻るため、Exitではなくタイトルシーンへ変更する処理を追加する予定
		// Exit状態に変更
		gameScene_->ChangState(std::make_unique<GameSceneExitState>());
	}
}

///-------------------------------------------/// 
/// 描画処理
///-------------------------------------------///
void GameSceneGameOverAnimationState::Draw() {
	// GameOverAnimationの描画
	gameOverAnimation_->Draw();
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void GameSceneGameOverAnimationState::Finalize() {
	gameOverAnimation_.reset();
	GameSceneFadeState::Finalize();
}
