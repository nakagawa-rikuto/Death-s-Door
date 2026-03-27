#include "GameSceneInitializeState.h"
// GameScene
#include "application/Scene/Game/GameScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
// Service
#include "Service/PostEffect.h"
// State
#include "GameSceneStartAnimationState.h"

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void GameSceneInitializeState::Enter(GameScene* gameScene) {
	// ゲームシーンのポインタを保存
	gameScene_ = gameScene;

	/// ===Transition=== ///
	float fadeInDuration = 2.0f;
	sceneManager_->StartFadeIn(TransitionType::ShatterGlass, fadeInDuration); // フェードイン開始
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void GameSceneInitializeState::Update() {
	// EnemyManagerの更新
	gameScene_->GetEnemy()->UpdateAnimation();
	// Playerの更新
	gameScene_->GetPlayer()->UpdateAnimation();

	if (sceneManager_->GetTransitionFinished()) {
		Service::PostEffect::SetOffScreenType(MiiEngine::OffScreenType::CopyImage);
		// InGame状態に変更
		gameScene_->ChangState(std::make_unique<GameSceneStartAnimationState>());
	}
}

///-------------------------------------------/// 
/// 描画処理
///-------------------------------------------///
void GameSceneInitializeState::Draw() {}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void GameSceneInitializeState::Finalize() {
	GameSceneFadeState::Finalize();
}
