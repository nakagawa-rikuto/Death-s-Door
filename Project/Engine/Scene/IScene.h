#pragma once
/// ===Include=== ///
#include "Engine/Camera/NormalCamera.h"
#include "Engine/Camera/FollowCamera.h"
#include "Engine/DataInfo/LevelData.h"
#include "Engine/DataInfo/BlendModeData.h"
// C++
#include <vector>
#include <memory>

namespace MiiEngine {
	/// ===前方宣言=== ///
	class SceneManager;

	///=====================================================/// 
	/// シーンの基底クラス
	///=====================================================///
	class IScene {
	public:

		IScene() = default;
		virtual ~IScene();

		virtual void Initialize();
		virtual void Update() = 0;
		virtual void Draw() = 0;

	public:/// ===Setter=== ///
		// SceneManager
		void SetSceneManager(SceneManager* sceneManager);

	protected:

		// シーンマネージャ
		SceneManager* sceneManager_ = nullptr;
		// Camera
		std::unique_ptr<NormalCamera> defaultCamera_;
	};
}

