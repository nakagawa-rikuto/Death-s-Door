#include "GameSceneGameOverAnimationState.h"
// GameScene
#include "application/Scene/Game/GameScene.h"
// Service
#include "Service/Camera.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
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
	gameScene_->GetEnemy()->UpdateAnimation();
	// Playerの更新
	gameScene_->GetPlayer()->Update();

	// アニメーション完了を確認
	if (gameOverAnimation_->IsCompleted() && !isTransitionStarted_) {
		// シーンマネージャーでフェードインを開始
		sceneManager_->StartFadeOut(TransitionType::BlackOut, 1.0f);
		isTransitionStarted_ = true;
	}

	/// ===シーンの切り替え=== ///
	if (isTransitionStarted_ && sceneManager_->GetTransitionFinished()) {
		// タイトルへ遷移
		sceneManager_->ChangeScene(MiiEngine::SceneType::Title);
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
