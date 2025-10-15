#include "Mii.h"

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
	rtvManager_->CreateSwapChainRenderTargets();

	// DSVManagerの生成
	dsvManager_ = std::make_unique<DSVManager>();
	dsvManager_->Initialize(dXCommon_.get());
	dsvManager_->CreateDepthBuffer(0);

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

	// Audiomanagerの生成
	audioManager_ = std::make_unique<AudioManager>();
	audioManager_->Initialze();

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
}

///=====================================================/// 
/// 更新
///=====================================================///
void Mii::Update() {
	// Input
	keyboard_->Update();
	mouse_->Update();
	controller_->Update();
	// ImGui
	imGuiManager_->Begin();
	
#ifdef USE_IMGUI
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

	// Manager
	levelManager_.reset();		// LevelManager
	csvManager_.reset();		// CSVManager
	audioManager_.reset();		// AudioManager
	animationManager_.reset();	// AnimationManager
	modelManager_.reset();		// Modelmanager
	textureManager_.reset();	// TextrureManager
	
	// SceneView
	sceneView_.reset();
	// OffScreen
	offScreenRenderer_.reset();
	// Pipline
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
	dXCommon_->PreDrawRenderTexture(offScreenRenderer_->GetBuffer());

	// 描画先のRTVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvManager_->GetCPUDescriptorHandle(0); // 通常DSVは1つ
	// OffScreenでRTVとDSVをセットしてRTVをクリアしている
	offScreenRenderer_->PreDraw(commandList, dsvHandle);
	// DSVはOffScreenで使用していないのでここでクリア
	dsvManager_->ClearDepthBuffer(commandList);

	// コマンドを積む
	dXCommon_->BeginCommand();

	// ディスクリプタヒープをバインド
	srvManager_->PreDraw();
}


///=====================================================/// 
/// フレーム終了処理
///=====================================================///
void Mii::EndFrame() {

	// ImGuiの開始処理
	//NOTE:ここはswapChainで設定
	dXCommon_->PreDrawImGui(rtvManager_.get());
	// バリアの状態遷移
	dXCommon_->TransitionRenderTarget();
	// OffScreen
	offScreenRenderer_->Draw(dXCommon_->GetCommandList());
	// ImGuiの開始処理
	imGuiManager_->Draw();
	// ImGuiの終了処理
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
SRVManager* Mii::GetSRVManager() {return srvManager_.get();}
// RTVManager
RTVManager* Mii::GetRTVManager() { return rtvManager_.get(); }
// DSVManager
DSVManager* Mii::GetDSVManager() { return dsvManager_.get(); }
// PiplelineManager
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
#pragma endregion
