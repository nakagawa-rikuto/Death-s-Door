#include "SceneManager.h"
// c++
#include <cassert>
// 各シーン
#include "Engine/Scene/DebugScene.h"
#include "application/Scene/TitleScene.h"
#include "application/Scene/SelectScene.h"
#include "application/Scene/GameScene.h"
#include "application/Scene/ClearScene.h"
#include "application/Scene/GameOverScene.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
SceneManager::~SceneManager() {
	currentScene_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void SceneManager::Initialize() {
	// シーンの確認
	if (currentScene_) {
		// 現在のシーンの初期化
		currentScene_->Initialize();
	}
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void SceneManager::Update() {
	// シーンの確認
	if (currentScene_) {
		// 現在のシーンの更新
		currentScene_->Update();
	}

	// ImGui
	SceneObservation();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void SceneManager::Draw() {
	// シーンの確認
	assert(currentScene_);
	if (currentScene_) {
		// 現在のシーンの描画
		currentScene_->Draw();
	}
}

///-------------------------------------------/// 
/// シーン変更
///-------------------------------------------///
void SceneManager::ChangeScene(SceneType type) {
	// 現在のシーンを更新
	currentSceneType_ = type;
	// 新しいシーンを生成
	if (currentScene_) { currentScene_.reset(); }
	currentScene_ = sceneFactory_->CreateScene(currentSceneType_);
	// 新しいシーンにSceneManagerをセット
	if (currentScene_) {
		currentScene_->SetSceneManager(this);
	}
	Initialize();
}

///-------------------------------------------/// 
/// シーン監視
///-------------------------------------------///
void SceneManager::SceneObservation() {
#ifdef USE_IMGUI
	ImGui::Begin("Scene Manager");
	/// ===現在のシーンを表示=== /// 
	if (currentScene_) {
		std::string currentSceneName = typeid(*currentScene_).name();
		ImGui::Text("Current Scene: %s", currentSceneName.c_str());
	} else {
		ImGui::Text("No Current Scene");
	}

	/// ===シーン変更用のドロップダウンメニュー=== ///
	if (ImGui::BeginCombo("Select Scene", "Change Scene")) {
		///タイトルシーン
		if (ImGui::Selectable("Title Scene", currentSceneType_ == SceneType::Title)) {
			ChangeScene(SceneType::Title);
		}
		///セレクトシーン
		if (ImGui::Selectable("Select Scene", currentSceneType_ == SceneType::Select)) {
			ChangeScene(SceneType::Select);
		}
		///ゲームシーン
		if (ImGui::Selectable("Game Scene", currentSceneType_ == SceneType::Game)) {
			ChangeScene(SceneType::Game);
		}
		///クリアシーン
		if (ImGui::Selectable("Clear Scene", currentSceneType_ == SceneType::Clear)) {
			ChangeScene(SceneType::Clear);
		}
		///ゲームオーバー
		if (ImGui::Selectable("GameOver Scene", currentSceneType_ == SceneType::GameOver)) {
			ChangeScene(SceneType::GameOver);
		}
		///デバッグシーン
		if (ImGui::Selectable("Debug Scene", currentSceneType_ == SceneType::Debug)) {
			ChangeScene(SceneType::Debug);
		}
		ImGui::EndCombo();
	}
	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void SceneManager::SetSelectedLevel(int level) { selectLevel_ = level; }
void SceneManager::SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
int SceneManager::GetSelectedLevel() const { return selectLevel_; }
