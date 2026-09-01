#include "SceneFactory.h"
// 各シーン
#include "application/Scene/Title/TitleScene.h"
#include "application/Scene/Select/SelectScene.h"
#include "application/Scene/Game/GameScene.h"
#include "application/Scene/Clear/ClearScene.h"
#include "application/Scene/GameOver/GameOverScene.h"
#ifdef _DEBUG
#include "Engine/Scene/ParticleEditor/ParticleEditorScene.h"
#include "application/Scene/Debug/AttackEditorScene.h"
#endif // _DEBUG

namespace MiiEngine {
	///-------------------------------------------/// 
	/// シーン生成
	///-------------------------------------------///
	std::unique_ptr<IScene> SceneFactory::CreateScene(SceneType type) {
		/// ===シーンの判断=== ///
		if (type == SceneType::Title) {
			// タイトルシーン
			return std::make_unique<TitleScene>();
		} else if (type == SceneType::Select) {
			// セレクトシーン
			return std::make_unique<SelectScene>();
		} else if (type == SceneType::Game) {
			//　ゲームシーン
			return std::make_unique<GameScene>();
		} else if (type == SceneType::Clear) {
			//　クリアシーン
			return std::make_unique<ClearScene>();
		} else if (type == SceneType::GameOver) {
			//　ゲームオーバー
			return std::make_unique<GameOverScene>();
#ifdef _DEBUG
		} else if (type == SceneType::ParticleEditor) {
			// 粒子エディターシーン
			return std::make_unique<ParticleEditorScene>();
		} else if (type == SceneType::AttackEditor) {
			// 攻撃エディターシーン
			return std::make_unique<AttackEditorScene>();
#endif // _DEBUG
		}
		return nullptr;
	}
}
