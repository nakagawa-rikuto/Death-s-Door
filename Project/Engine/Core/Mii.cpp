#include "Mii.h"

namespace MiiEngine {
	///=====================================================/// 
	/// システム全体の初期化
	///=====================================================///
	void Mii::Initialize(const wchar_t* title, int width, int height) {

		// ゲームウィンドウの作成
		winApp_ = std::make_unique<WinApp>();
		winApp_->CreateGameWindow(title, width, height);

		// DirectXの生成
		dXCommon_ = std::make_unique<DXCommon>();
		dXCommon_->Initialize(winApp_.get(), width, height);

		// SRVManagerの生成
		srvManager_ = std::make_unique<SRVManager>();
		srvManager_->Initialize(dXCommon_.get());

		// RTVManagerの生成
		rtvManager_ = std::make_unique<RTVManager>();
		rtvManager_->Initialize(dXCommon_.get());
		rtvManager_->CreateSwapChainRenderTargetView();

		// DSVManagerの生成
		dsvManager_ = std::make_unique<DSVManager>();
		dsvManager_->Initialize(dXCommon_.get(), winApp_.get());
		dsvManager_->CreateDepthBufferView(0);

		// ImGuiManagerの生成
		imGuiManager_ = std::make_unique<ImGuiManager>();
		imGuiManager_->Initialize(winApp_.get(), dXCommon_.get(), srvManager_.get());

		// PipelineManagerの生成
		pipelineManager_ = std::make_unique<PipelineManager>();
		pipelineManager_->Initialize(dXCommon_.get());

		// OffScreenRendererの生成
		offScreenRenderer_ = std::make_unique<OffScreenRenderer>();
		offScreenRenderer_->Initialize(
			dXCommon_->GetDevice(),
			srvManager_.get(), rtvManager_.get(),
			width, height, Vector4(0.47f, 0.81f, 0.62f, 1.0f)); // クリアカラーをここで設定

		// SceneViewの生成
		sceneView_ = std::make_unique<SceneView>();
		sceneView_->SetTextureHandle(offScreenRenderer_->GetResultSRV());
		imGuiManager_->SetSceneView(sceneView_.get());

		// TextureManagerの生成
		textureManager_ = std::make_unique<TextureManager>();
		textureManager_->Initialize(dXCommon_.get(), srvManager_.get());

		// ModelManagerの生成	
		modelManager_ = std::make_unique<ModelManager>();
		modelManager_->Initialize(textureManager_.get());

		// AnimationManagerの生成
		animationManager_ = std::make_unique<AnimationManager>();

		// AudioManagerの生成
		audioManager_ = std::make_unique<AudioManager>();
		audioManager_->Initialize();

		// CSVManagerの生成
		csvManager_ = std::make_unique<CSVManager>();

		// LevelManagerの生成
		levelManager_ = std::make_unique<LevelManager>();

		// LineObject3Dの生成
		lineObject3D_ = std::make_unique<LineObject3D>();
		lineObject3D_->Initialize(dXCommon_->GetDevice());

		// InputCommonの生成
		inputCommon_ = std::make_unique<InputCommon>();
		inputCommon_->Initialize(winApp_.get());

		// Keyboardの生成
		keyboard_ = std::make_unique<Keyboard>();
		keyboard_->Initialize(winApp_.get(), inputCommon_->GetDirectInput().Get());

		// Mouseの生成
		mouse_ = std::make_unique<Mouse>();
		mouse_->Initialize(winApp_.get(), inputCommon_->GetDirectInput().Get());

		// Controllerの生成
		controller_ = std::make_unique<Controller>();
		controller_->Initialize();

		// InputTrackerの生成
		inputTracker_ = std::make_unique<InputTracker>();
	}

	///=====================================================/// 
	/// 更新
	///=====================================================///
	void Mii::Update() {
		// ImGui
		imGuiManager_->Begin();

		// Input
		keyboard_->Update();
		mouse_->Update();
		controller_->Update();
		inputTracker_->Update(*keyboard_, *mouse_, *controller_);

#ifdef USE_IMGUI
		// FPSの計測・表示
		ImGui::Begin("System Info");
		ImGui::Text("FPS: %.1f", dXCommon_->GetFPS());
		ImGui::End();

		// OffScreenRendererのImGui
		offScreenRenderer_->DrawImGui();
#endif // USE_IMGUI
	}

	///=====================================================/// 
	/// システム全体の終了
	///=====================================================///
	void Mii::Finalize() {
		// 読み込んだ音声データの一括停止・解放
		audioManager_->StopAll();
		audioManager_->UnloadAll();
		// LineObject3D
		lineObject3D_->Reset();
		// ImGuiの終了処理
		imGuiManager_->Finalize();
		// ゲームウィンドウの破棄
		winApp_->TerminateGameWindow();

		// 手動の解放
		// Input
		controller_.reset();		// Controller
		mouse_.reset();				// Mouse
		keyboard_.reset();			// Keyboard
		inputCommon_.reset();		// InputCommon
		lineObject3D_.reset();      // LineObject3D
		inputTracker_.reset();      // InputTracker

		// Manager
		levelManager_.reset();		// LevelManager
		csvManager_.reset();		// CSVManager
		audioManager_.reset();		// AudioManager
		animationManager_.reset();	// AnimationManager
		modelManager_.reset();		// ModelManager
		textureManager_.reset();	// TextureManager

		// SceneView
		sceneView_.reset();
		// OffScreen
		offScreenRenderer_.reset();
		// PipelineManager
		pipelineManager_.reset();
		// ImGui
		imGuiManager_.reset();
		// SRV, RTV, DSV
		dsvManager_.reset();		// DSVManager
		rtvManager_.reset();		// RTVManager
		srvManager_.reset();		// SRVManager
		// DXCommon
		dXCommon_.reset();
		// WinApp
		winApp_.reset();

		// COMの終了
		CoUninitialize();
	}


	///=====================================================/// 
	/// フレーム開始処理
	///=====================================================///
	void Mii::BeginFrame() {
		// 描画前処理
		// CommandListの取得
		ID3D12GraphicsCommandList* commandList = dXCommon_->GetCommandList();

		// Barrierの設定
		dXCommon_->PreDrawRenderTexture(offScreenRenderer_->GetSceneBuffer());
		dXCommon_->PreDrawEffectTexture(offScreenRenderer_->GetEffectBuffer());

		// 描画先のRTVを設定する
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvManager_->GetCPUDescriptorHandle(0); // 通常DSVは1つ
		// OffScreenでRTVとDSVをセットしてRTVをクリアしている
		offScreenRenderer_->PreDraw(commandList, dsvHandle);
		// DSVはOffScreenで使用していないのでここでクリア
		dsvManager_->ClearDepthBufferView(commandList);

		// コマンドを積む
		dXCommon_->BeginCommand();

		// ディスクリプタヒープをバインド
		srvManager_->PreDraw();
	}


	///=====================================================/// 
	/// フレーム終了処理
	///=====================================================///
	void Mii::EndFrame() {
		// ImGuiの描画準備
		dXCommon_->PreDrawImGui(rtvManager_.get());

		// sceneTextureのバリアの状態遷移
		dXCommon_->TransitionRenderTarget();

		// OffScreenエフェクトをSwapChainに描画
		offScreenRenderer_->Draw(dXCommon_->GetCommandList());

		// EffectTextureのバリア状態遷移
		dXCommon_->TransitionEffectTexture();

		// effectTexture_ を SwapChain にコピー
		UINT backBufferIndex = dXCommon_->GetBackBufferIndex();
		D3D12_CPU_DESCRIPTOR_HANDLE swapChainRTV = rtvManager_->GetCPUDescriptorHandle(backBufferIndex);
		offScreenRenderer_->CopyToSwapChain(dXCommon_->GetCommandList(), swapChainRTV);

		// ImGuiのUI描画
		imGuiManager_->Draw();

		// ImGuiの描画実行
		imGuiManager_->End();

		// DXCommonの描画後処理
		dXCommon_->PostDraw();
	}


	///=====================================================/// 
	/// Windowsのメッセージを処理する
	///=====================================================///
	int Mii::ProcessMessage() { return winApp_->ProcessMessage(); }


	///-------------------------------------------/// 
	/// Getter
	///-------------------------------------------///
	#pragma region 開発者用
	// DXCommon
	DXCommon* Mii::GetDXCommon() { return dXCommon_.get(); }
	// WinApp
	WinApp* Mii::GetWinApp() { return winApp_.get(); }
	// SRVManager
	SRVManager* Mii::GetSRVManager() { return srvManager_.get(); }
	// RTVManager
	RTVManager* Mii::GetRTVManager() { return rtvManager_.get(); }
	// DSVManager
	DSVManager* Mii::GetDSVManager() { return dsvManager_.get(); }
	// PipelineManager
	PipelineManager* Mii::GetPipelineManager() { return pipelineManager_.get(); }
	// TextureManager
	TextureManager* Mii::GetTextureManager() { return textureManager_.get(); }
	// ModelManager
	ModelManager* Mii::GetModelManager() { return modelManager_.get(); }
	// AudioManager
	AudioManager* Mii::GetAudioManager() { return audioManager_.get(); }
	// CSVManager
	CSVManager* Mii::GetCSVManager() { return csvManager_.get(); }
	// LevelManager
	LevelManager* Mii::GetLevelManager() { return levelManager_.get(); }
	// AnimationManager
	AnimationManager* Mii::GetAnimationManager() { return animationManager_.get(); }
	// OffScreenRenderer
	OffScreenRenderer* Mii::GetOffScreenRenderer() { return offScreenRenderer_.get(); }
	// LineObject3D
	LineObject3D* Mii::GetLineObject3D() { return lineObject3D_.get(); }
	// Keyboard
	Keyboard* Mii::GetKeyboard() { return keyboard_.get(); }
	// Mouse
	Mouse* Mii::GetMouse() { return mouse_.get(); }
	// Controller
	Controller* Mii::GetController() { return controller_.get(); }
	// InputTracker
	InputTracker* Mii::GetInputTracker() { return inputTracker_.get(); }
	#pragma endregion
}