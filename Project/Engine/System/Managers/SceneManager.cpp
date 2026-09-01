#include "SceneManager.h"
// c++
#include <cassert>
// SceneTransitionManager
#include "Engine/Scene/Transition/Manager/SceneTransitionManager.h"
// SpriteManager
#include "SpriteManager.h"

namespace MiiEngine {
	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	SceneManager::~SceneManager() {
		currentScene_.reset();
		sceneTransitionManager_.reset();
	}

	///-------------------------------------------/// 
	/// Setter
	///-------------------------------------------///
	void SceneManager::SetSelectedLevel(int level) { selectLevel_ = level; }
	void SceneManager::SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

	///-------------------------------------------/// 
	/// Getter
	///-------------------------------------------///
	// 選択されたレベル番号の取得
	int SceneManager::GetSelectedLevel() const { return selectLevel_; }
	// トランジション完了の取得
	bool SceneManager::GetTransitionFinished() const { return sceneTransitionManager_->IsFinished(); }
	// フェード状態の取得
	FadeState SceneManager::GetFadeState() const { return sceneTransitionManager_->GetState(); }

	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	void SceneManager::Initialize(AbstractSceneFactory* sceneFactor, SpriteManager* spriteManager) {
		// シーンファクトリーのセット
		SetSceneFactory(sceneFactor);

		// シーントランジションマネージャの生成
		sceneTransitionManager_ = std::make_unique<SceneTransitionManager>();
		sceneTransitionManager_->Initialize();

		// SpriteManagerのセット
		spriteManager_ = spriteManager;
	}

	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void SceneManager::Update() {
		// フェードアウト完了後の新規作成
		if (sceneTransitionManager_->GetState() == FadeState::FadeIn && sceneTransitionManager_->IsFinished()) {
			sceneTransitionManager_->NewMake();
		}

		// シーントランジションマネージャの更新
		sceneTransitionManager_->Update();

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

		// シーントランジションマネージャの描画
		sceneTransitionManager_->Draw();
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
			sceneTransitionManager_->Reset();
		}
		SceneInit();
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
			/// タイトルシーン
			if (ImGui::Selectable("Title Scene", currentSceneType_ == SceneType::Title)) {
				ChangeScene(SceneType::Title);
			}
			/// セレクトシーン
			if (ImGui::Selectable("Select Scene", currentSceneType_ == SceneType::Select)) {
				ChangeScene(SceneType::Select);
			}
			/// ゲームシーン
			if (ImGui::Selectable("Game Scene", currentSceneType_ == SceneType::Game)) {
				ChangeScene(SceneType::Game);
			}
			/// クリアシーン
			if (ImGui::Selectable("Clear Scene", currentSceneType_ == SceneType::Clear)) {
				ChangeScene(SceneType::Clear);
			}
			/// ゲームオーバー
			if (ImGui::Selectable("GameOver Scene", currentSceneType_ == SceneType::GameOver)) {
				ChangeScene(SceneType::GameOver);
			}
			/// 粒子エディターシーン
			if (ImGui::Selectable("ParticleEditor", currentSceneType_ == SceneType::ParticleEditor)) {
				ChangeScene(SceneType::ParticleEditor);
			}
			// アタックエディターシーン
			if (ImGui::Selectable("AttackEditor", currentSceneType_ == SceneType::AttackEditor)) {
				ChangeScene(SceneType::AttackEditor);
			}
			ImGui::EndCombo();
		}
		ImGui::End();
	#endif // USE_IMGUI
	}

	///-------------------------------------------/// 
	/// フェードイン開始
	///-------------------------------------------///
	void SceneManager::StartFadeIn(TransitionType type, const float duration) {
		sceneTransitionManager_->StartFadeIn(type, duration);
	}

	///-------------------------------------------/// 
	/// フェードアウト開始
	///-------------------------------------------///
	void SceneManager::StartFadeOut(TransitionType type, const float duration) {
		sceneTransitionManager_->StartFadeOut(type, duration);
	}

	///-------------------------------------------/// 
	/// シーン初期化
	///-------------------------------------------///
	void SceneManager::SceneInit() {
		// シーンの確認
		if (currentScene_) {
			// 現在のシーンの初期化
			currentScene_->Initialize();
			// シーンの最初にスプライトを更新
			spriteManager_->Update();
		}
	}
}