#include "Locator.h"
// c++
#include <cassert>

namespace Service {
	///-------------------------------------------/// 
	/// 全てのサービスを提供
	///-------------------------------------------///
	void Locator::ProvideAll(const Register& registry) {
		/// ===nullチェック=== ///
		// WinApp
		assert(registry.winApp);
		// DXCommon
		assert(registry.dxCommon);
		// SRV, RTV, DSV
		assert(registry.srvManager);
		assert(registry.dsvManager);
		assert(registry.rtvManager);
		// PipelineManager
		assert(registry.pipelineManager);
		// TextureManager
		assert(registry.textureManager);
		// ModelManager
		assert(registry.modelManager);
		// AnimationManager
		assert(registry.animationManager);
		// AudioManager
		assert(registry.audioManager);
		// CSVManager
		assert(registry.csvManager);
		// LevelManager
		assert(registry.levelManager);
		// OffScreenRenderer
		assert(registry.offScreenRenderer);
		// LineObject3D
		assert(registry.lineObject3D);
		// Input
		assert(registry.keyboard);
		assert(registry.mouse);
		assert(registry.controller);
		assert(registry.inputTracker);
		// CameraManager
		assert(registry.cameraManager);
		// ParticleManager
		assert(registry.particleManager);
		// ColliderManager
		assert(registry.colliderManager);
		// SpriteManager
		assert(registry.spriteManager);
		// DeltaTime
		assert(registry.gameTime);

		/// ===代入=== ///
		// WinApp
		winApp_ = registry.winApp;
		// DXCommon
		dxCommon_ = registry.dxCommon;
		// SRV, RTV, DSV
		srvManager_ = registry.srvManager;
		rtvManager_ = registry.rtvManager;
		dsvManager_ = registry.dsvManager;
		// PipelineManager
		pipelineManager_ = registry.pipelineManager;
		// Texture
		textureManager_ = registry.textureManager;
		// Model
		modelManager_ = registry.modelManager;
		// Animation
		animationManager_ = registry.animationManager;
		// Audio
		audioManager_ = registry.audioManager;
		// CSV
		csvManager_ = registry.csvManager;
		// Level
		levelManager_ = registry.levelManager;
		// OffScreen
		offScreenRenderer_ = registry.offScreenRenderer;
		// LineObject
		lineObject3D_ = registry.lineObject3D;
		// Input
		keyboard_ = registry.keyboard;
		mouse_ = registry.mouse;
		controller_ = registry.controller;
		inputTracker_ = registry.inputTracker;
		// CameraManager
		cameraManager_ = registry.cameraManager;
		// ParticleManager
		particleManager_ = registry.particleManager;
		// ColliderManager
		colliderManager_ = registry.colliderManager;
		// SpriteManager
		spriteManager_ = registry.spriteManager;
		// DeltaTime
		gameTime_ = registry.gameTime;
	}

	///-------------------------------------------/// 
	/// 全てのサービスを終了
	///-------------------------------------------///
	void Locator::Finalize() {
		spriteManager_ = nullptr;
		colliderManager_ = nullptr;
		particleManager_ = nullptr;
		cameraManager_ = nullptr;
		controller_ = nullptr;
		mouse_ = nullptr;
		keyboard_ = nullptr;
		inputTracker_ = nullptr;
		levelManager_ = nullptr;
		csvManager_ = nullptr;
		audioManager_ = nullptr;
		lineObject3D_ = nullptr;
		offScreenRenderer_ = nullptr;
		animationManager_ = nullptr;
		modelManager_ = nullptr;
		textureManager_ = nullptr;
		pipelineManager_ = nullptr;
		dsvManager_ = nullptr;
		rtvManager_ = nullptr;
		srvManager_ = nullptr;
		gameTime_ = nullptr;
		winApp_ = nullptr;
		dxCommon_ = nullptr;
	}

	///-------------------------------------------/// 
	/// DXCommon
	///-------------------------------------------///
	MiiEngine::DXCommon* Locator::GetDXCommon() { return dxCommon_; }

	///-------------------------------------------/// 
	/// WinApp
	///-------------------------------------------///
	MiiEngine::WinApp* Locator::GetWinApp() { return winApp_; }

	///-------------------------------------------/// 
	/// SRVManager
	///-------------------------------------------///
	MiiEngine::SRVManager* Locator::GetSRVManager() { return srvManager_; }

	///-------------------------------------------/// 
	/// RTVManager
	///-------------------------------------------///
	MiiEngine::RTVManager* Locator::GetRTVManager() { return rtvManager_; }

	///-------------------------------------------/// 
	/// DSVManager
	///-------------------------------------------///
	MiiEngine::DSVManager* Locator::GetDSVManager() { return dsvManager_; }

	///-------------------------------------------/// 
	/// PipelineManager
	///-------------------------------------------///
	MiiEngine::PipelineManager* Locator::GetPipelineManager() { return pipelineManager_; }

	///-------------------------------------------/// 
	/// TextureManager
	///-------------------------------------------///
	MiiEngine::TextureManager* Locator::GetTextureManager() { return textureManager_; }

	///-------------------------------------------/// 
	/// ModelManager
	///-------------------------------------------///
	MiiEngine::ModelManager* Locator::GetModelManager() { return modelManager_; }

	///-------------------------------------------/// 
	/// AnimationManager
	///-------------------------------------------///
	MiiEngine::AnimationManager* Locator::GetAnimationManager() { return animationManager_; }

	///-------------------------------------------/// 
	/// CSVManager
	///-------------------------------------------///
	MiiEngine::CSVManager* Locator::GetCSVManager() { return csvManager_; }

	///-------------------------------------------/// 
	/// LevelManager
	///-------------------------------------------///
	MiiEngine::LevelManager* Locator::GetLevelManager() { return levelManager_; }

	///-------------------------------------------/// 
	/// AudioManager
	///-------------------------------------------///
	MiiEngine::AudioManager* Locator::GetAudioManager() { return audioManager_; }

	///-------------------------------------------/// 
	/// LineObject3D
	///-------------------------------------------///
	MiiEngine::LineObject3D* Locator::GetLineObject3D() { return lineObject3D_; }

	///-------------------------------------------/// 
	/// OffScreenRenderer
	///-------------------------------------------///
	MiiEngine::OffScreenRenderer* Locator::GetOffScreenRenderer() { return offScreenRenderer_; }

	///-------------------------------------------/// 
	/// Input
	///-------------------------------------------///
	MiiEngine::Keyboard* Locator::GetKeyboard() { return keyboard_; }
	MiiEngine::Mouse* Locator::GetMouse() { return mouse_; }
	MiiEngine::Controller* Locator::GetController() { return controller_; }
	MiiEngine::InputTracker* Locator::GetInputTracker() { return inputTracker_; }

	///-------------------------------------------/// 
	/// CameraManager
	///-------------------------------------------///
	MiiEngine::CameraManager* Locator::GetCameraManager() { return cameraManager_; }

	///-------------------------------------------/// 
	/// particleManager
	///-------------------------------------------///
	MiiEngine::ParticleManager* Locator::GetParticleManager() { return particleManager_; }

	///-------------------------------------------/// 
	/// ColliderManager
	///-------------------------------------------///
	MiiEngine::ColliderManager* Locator::GetColliderManager() { return colliderManager_; }

	///-------------------------------------------/// 
	/// SpriteManager
	///-------------------------------------------///
	MiiEngine::SpriteManager* Locator::GetSpriteManager() { return spriteManager_; }

	///-------------------------------------------/// 
	/// GameTime
	///-------------------------------------------///
	MiiEngine::GameTime* Locator::GetGameTime() { return gameTime_; }
}