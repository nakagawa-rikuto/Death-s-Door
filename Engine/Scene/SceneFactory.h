#pragma once
/// ===Include=== ///
#include "Engine/Scene/AbstractSceneFactory.h"

///=====================================================/// 
/// シーン工場
///=====================================================///
class SceneFactory : public AbstractSceneFactory {
public:
	// シーン生成
	std::unique_ptr<IScene> CreateScene(SceneType type) override;
};

