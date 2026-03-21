#include "GameSceneGameClearState.h"
// GameScene
#include "application/Scene/Game/GameScene.h"
// Service
#include "Service/Camera.h"
// State
#include "GameSceneExitState.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
GameSceneGameClearState::~GameSceneGameClearState() {
	gameClearAnimation_.reset();
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void GameSceneGameClearState::Enter(GameScene* gameScene) {
	// ゲームシーンのポインタを保存
	gameScene_ = gameScene;

	/// ===GameClearAnimation=== ///
	gameClearAnimation_ = std::make_unique<GameClearAnimation>();
	gameClearAnimation_->Initialize(gameScene_->GetPlayer(), gameScene_->GetCamera());
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void GameSceneGameClearState::Update() {
	// GameClearAnimationの更新
	gameClearAnimation_->Update();
	// EnemyManagerの更新
	gameScene_->GetEnemyManager()->UpdateAnimation();
	// Playerの更新
	gameScene_->GetPlayer()->UpdateAnimation();
	// アニメーション完了を確認
	if (gameClearAnimation_->IsCompleted()) {
		// Exit状態に変更
		gameScene_->ChangState(std::make_unique<GameSceneExitState>());
	}
}

///-------------------------------------------/// 
/// 描画処理
///-------------------------------------------///
void GameSceneGameClearState::Draw() {
	// GameClearAnimationの描画
	gameClearAnimation_->Draw();
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void GameSceneGameClearState::Finalize() {
	gameClearAnimation_.reset();
	GameSceneFadeState::Finalize();
}
