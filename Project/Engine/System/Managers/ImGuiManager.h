#pragma once
/// ===Include=== ///
#include "Engine/Core/ComPtr.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#endif // USE_IMGUI



/// ===前方宣言=== ///
class WinApp;
class DXCommon;
class SRVManager;
class SceneView;

///=====================================================/// 
/// ImGuiManager
///=====================================================///
class ImGuiManager {
public:

	ImGuiManager();
	~ImGuiManager();

	// 初期化
	void Initialize(WinApp* winApp, DXCommon* dxCommon, SRVManager* srvManager);
	// 終了
	void Finalize();
	// 受付開始処理
	void Begin();
	// 受付終了処理
	void End();
	// 画面への描画処理
	void Draw();
	
	// Setter
	void SetSceneView(SceneView* sceneView);

private:
	
	WinApp* winApp_ = nullptr;
	DXCommon* dxCommon_ = nullptr;
	SRVManager* srvManager_ = nullptr;
	SceneView* sceneView_ = nullptr;

private:
	void DrawMainDockWindow(); // Dock付きUI描画
	void MenuBar();            // メニューバーのみ
};

