#pragma once
/// ===Include=== ///
// c++
#include <memory>
#include <string>
// Game
#include "Engine/Scene/AbstractSceneFactory.h"
// Imgui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

///=====================================================/// 
/// シーンマネージャー
///=====================================================///
class SceneManager {
public:/// ===基本的な関数=== ///

	SceneManager() = default;
	~SceneManager();

	// 初期化
	void Initialize();
	// 現在のシーンの更新
	void Update();
	// 現在のシーンの描画
	void Draw();

public:/// ===シーン処理=== ///

	// シーンの変更
	void ChangeScene(SceneType type);
	// シーン監視
	void SceneObservation();
	// レベル番号の設定
	void SetSelectedLevel(int level);
	// 選択されたレベル番号の取得
	int GetSelectedLevel() const;
	// シーンファクトリーのSetter
	void SetSceneFactory(AbstractSceneFactory* sceneFactory);

private:
	// シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;

	// 現在のシーン
	SceneType currentSceneType_ = SceneType::Debug; // 初期のシーン
	std::unique_ptr<IScene> currentScene_; // 現在のシーン

	// 選択されたレベル番号を保持する
	int selectLevel_ = 1;
};

