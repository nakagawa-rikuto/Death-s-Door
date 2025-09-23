#pragma once
/// ===Include=== ///
#include "application/Game/Camera/GameCamera.h"
#include "Engine/DataInfo/LevelData.h"
#include "Engine/DataInfo/BlendModeData.h"

#include <vector>

/// ===前方宣言=== ///
class SceneManager;
class Model;

///=====================================================/// 
/// シーンの基底クラス
///=====================================================///
class IScene {
public:

	IScene() = default;
	virtual ~IScene();

	virtual void Initialize();
	virtual void Update();
	virtual void Draw();

public: /// ===Getter=== ///

	const float GetDeltaTime()const;

public:/// ===Setter=== ///

	void SetSceneManager(SceneManager* sceneManager);
	void SetDeltaTime(const float deltaTime);

protected:
	// 時間の経過速度
	float deltaTime_;

	// シーンマネージャ
	SceneManager* sceneManager_ = nullptr;
	// Camera
	std::shared_ptr<GameCamera> defaultCamera_;

	// 共有モデル群
	std::vector<std::unique_ptr<Model>> models_;

	// 配置関数(Level)
	void GenerateModelsFromLevelData(const std::string& file_name);
	void UpdateLevelModels();
	void DrawLevelModels(BlendMode mode = BlendMode::kBlendModeNone);
};

