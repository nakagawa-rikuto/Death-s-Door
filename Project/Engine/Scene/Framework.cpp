#include "Framework.h"
// Service
#include "Service/Locator.h"

namespace MiiEngine {
	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	void Framework::Initialize(const wchar_t* title) {
		/// ===ウィンドウサイズの設定=== ///
#ifdef _DEBUG
		width_ = 1920;
		height_ = 1080;
#else
		width_ = 1600;
		height_ = 900;
#endif // DEBUG

		/// ===初期化=== ///
		// MiiEngine
		Engine_ = std::make_unique<Mii>();
		Engine_->Initialize(title, width_, height_);
		// CameraManager
		cameraManager_ = std::make_unique<CameraManager>();
		// ParticleManager
		particleManager_ = std::make_unique<ParticleManager>();
		// ColliderManager
		colliderManager_ = std::make_unique<ColliderManager>();
		colliderManager_->Initialize();
		// SpriteManager
		spriteManager_ = std::make_unique<SpriteManager>();
		// DeltaTime
		gameTime_ = std::make_unique<GameTime>();

		// Locator
		Service::Register registry = {
			Engine_->GetWinApp(),
			Engine_->GetDXCommon(),
			Engine_->GetSRVManager(),
			Engine_->GetRTVManager(),
			Engine_->GetDSVManager(),
			Engine_->GetPipelineManager(),
			Engine_->GetTextureManager(),
			Engine_->GetModelManager(),
			Engine_->GetAnimationManager(),
			Engine_->GetOffScreenRenderer(),
			Engine_->GetAudioManager(),
			Engine_->GetCSVManager(),
			Engine_->GetLevelManager(),
			Engine_->GetLineObject3D(),
			Engine_->GetKeyboard(),
			Engine_->GetMouse(),
			Engine_->GetController(),
			Engine_->GetInputTracker(),
			cameraManager_.get(),
			particleManager_.get(),
			colliderManager_.get(),
			spriteManager_.get(),
			gameTime_.get()
		};
		Service::Locator::ProvideAll(registry);
	}

	///-------------------------------------------/// 
	/// 終了
	///-------------------------------------------///
	void Framework::Finalize() {
		/// ===終了処理=== ///
		// サービスロケータ
		Service::Locator::Finalize();
		// CameraManager
		cameraManager_.reset();
		// ParticleManager
		particleManager_.reset();
		// ColliderManager
		colliderManager_.reset();
		// SpriteManager
		spriteManager_.reset();
		// MiiEngine
		Engine_->Finalize();
		Engine_.reset();
	}

	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void Framework::Update() {
		/// ===システムの更新処理=== ///
		// CameraManager
		cameraManager_->UpdateAllCameras();
		// Line
		Engine_->GetLineObject3D()->Update();
		// ParticleManager
		particleManager_->Update();
		// SpriteManager
		spriteManager_->Update();
		// ColliderManager
		colliderManager_->CheckAllCollisions();
		
	}

	///-------------------------------------------/// 
	/// 終了チェック
	///-------------------------------------------///
	bool Framework::IsEndRequest() { return endRequest_; }

	///-------------------------------------------/// 
	/// 実行
	///-------------------------------------------///
	void Framework::Run(const wchar_t* title) {
		/// ===ゲームの初期化=== ///
		Initialize(title);
		// ウィンドウのxボタンが押されるまでループ
		while (Engine_->ProcessMessage() == 0) {
			/// ===毎フレーム更新=== ///
			// MiiEngineの更新処理
			Engine_->Update();
			// フレームごとの更新処理
			Update();
			/// ===終了リクエストが来たら抜ける=== ///
			if (IsEndRequest()) { break; }
			/// ===描画=== ///
			Draw();
		}
		/// ===ゲーム終了=== ///
		Finalize();
	}

	///-------------------------------------------/// 
	/// 描画前処理
	///-------------------------------------------///
	void Framework::PreDraw() {
		// フレームの開始
		Engine_->BeginFrame();
		// SpriteManager
		spriteManager_->BackDraw();
		// Lineの描画
		Engine_->GetLineObject3D()->Draw();
	}

	///-------------------------------------------/// 
	/// 描画後処理
	///-------------------------------------------///
	void Framework::PostDraw() {
		// ParticleManager
		particleManager_->Draw(BlendMode::kBlendModeAdd);
		// SpriteManager
		spriteManager_->FrontDraw();
		// フレームの終了
		Engine_->EndFrame();
	}
}
