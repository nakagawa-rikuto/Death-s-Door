#pragma once
/// ===include=== ///
// C++
#include <memory>
// Engine
#include "Engine/Core/WinApp.h"
#include "Engine/Core/DXCommon.h"
// Input
#include "Engine/System/Input/InputCommon.h"
#include "Engine/System/Input/Keyboard.h"
#include "Engine/System/Input/Mouse.h"
#include "Engine/System/Input/Controller.h"
// Manager
#include "Engine/System/Managers/SRVManager.h"
#include "Engine/System/Managers/RTVManager.h"
#include "Engine/System/Managers/DSVManager.h"
#include "Engine/System/Managers/PiplineManager.h"
#include "Engine/System/Managers/TextureManager.h"
#include "Engine/System/Managers/ModelManager.h"
#include "Engine/System/Managers/ImGuiManager.h"
#include "Engine/System/Managers/AudioManager.h"
#include "Engine/System/Managers/CSVManager.h"
#include "Engine/System/Managers/AnimationManager.h"
#include "Engine/System/Managers/LevelManager.h"
// OffScreenRender
#include "Engine/Graphics/OffScreen/OffScreenRenderer.h"
// ImGui
#include "Engine/System/ImGui/SceneView.h"
// LineObject
#include "Engine/Graphics/3d/Line/LineObject3D.h"


///=====================================================///
/// システム
///=====================================================///
class Mii {
public:

	Mii() = default;
	~Mii() = default;

public:/// ===開発者用関数(システム)=== ///

	// システム全体の初期化
	void Initialize(const wchar_t* title, int width = 1920, int height = 1080);
	// システム全体の更新
	void Update();
	// システム全体の終了
	void Finalize();
	// フレーム開始処理
	void BeginFrame();
	// フレーム終了処理
	void EndFrame();
	// Windowsのメッセージを処理
	int ProcessMessage();

public:/// ===開発者用関数(Getter)=== ///
	// DXCommonの取得
	DXCommon* GetDXCommon();
	// WinAppの取得
	WinApp* GetWinApp();
	// SRVManagerの取得
	SRVManager* GetSRVManager();
	// RTVManagerの取得
	RTVManager* GetRTVManager();
	// DSVManagerの取得
	DSVManager* GetDSVManager();
	// PiplelineManagerの取得
	PipelineManager* GetPipelineManager();
	// TextureManagerの取得
	TextureManager* GetTextureManager();
	// ModelManagerの取得
	ModelManager* GetModelManager();
	// AudioManagerの取得
	AudioManager* GetAudioManager();
	// CSVManagerの取得
	CSVManager* GetCSVManager();
	// LevelManagerの取得
	LevelManager* GetLevelManager();
	// AnimationManagerの取得
	AnimationManager* GetAnimationManager();
	// OffScreenRendererの取得
	OffScreenRenderer* GetOffScreenRenderer();
	// LineObject3Dの取得
	LineObject3D* GetLineObject3D();
	// Keyboardの取得
	Keyboard* GetKeyboard();
	// Mouseの取得
	Mouse* GetMouse();
	// Controllerの取得
	Controller* GetController();

private:/// ===Variables(変数)=== ///
	// Core
	std::unique_ptr<WinApp> winApp_;                     // WinApp
	std::unique_ptr<DXCommon> dXCommon_;                 // DirectXCommon
	// Input
	std::unique_ptr<InputCommon> inputCommon_;           // inputCommon
	std::unique_ptr<Keyboard> keyboard_;                 // Keyboard
	std::unique_ptr<Mouse> mouse_;                       // Mouse
	std::unique_ptr<Controller> controller_;             // Controller
	// Manager
	std::unique_ptr<SRVManager> srvManager_;             // SRVManager
	std::unique_ptr<RTVManager> rtvManager_;             // RTVManager
	std::unique_ptr<DSVManager> dsvManager_;             // DSVManager
	std::unique_ptr<PipelineManager> pipelineManager_;   // PipelineManager
	std::unique_ptr<TextureManager> textureManager_;     // TextureManager
	std::unique_ptr<ModelManager> modelManager_;         // ModelManager
	std::unique_ptr<ImGuiManager> imGuiManager_;         // ImGuiManager
	std::unique_ptr<AudioManager> audioManager_;         // AudioMangaer
	std::unique_ptr<CSVManager> csvManager_;             // CSVManager
	std::unique_ptr<AnimationManager> animationManager_; // AnimationManager
	// OffScreen
	std::unique_ptr<OffScreenRenderer> offScreenRenderer_;// OffScreen
	// ImGui
	std::unique_ptr<SceneView> sceneView_;                // SceneView
	// Line
	std::unique_ptr<LineObject3D> lineObject3D_;          // LineObject3D
	// Level
	std::unique_ptr<LevelManager> levelManager_;			  // LevelLoader

private:
	///=====================================================/// 
	/// ReportLiveObjects()
	///=====================================================///
	struct D3DResourceLeakChecker {

		~D3DResourceLeakChecker() {

			// リソースリークチェック
			ComPtr<IDXGIDebug1> debug;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
				debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			}
		}
	};

	// ReportLiveObjects
	D3DResourceLeakChecker leakCheck;
};

