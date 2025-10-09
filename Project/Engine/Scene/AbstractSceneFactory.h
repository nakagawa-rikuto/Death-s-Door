#pragma once
/// ===Include=== ///
#include "Engine/Scene/IScene.h"
#include "Engine/DataInfo/SceneType.h"
#include <memory>
#include <string>

///=====================================================/// 
/// シーン工場（概念）
///=====================================================///
class AbstractSceneFactory {
public:
	// 仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;
	// シーン生成
	virtual std::unique_ptr<IScene> CreateScene(SceneType type) = 0;
};

