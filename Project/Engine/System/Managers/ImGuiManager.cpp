#include "ImGuiManager.h"
// Engine
#include "Engine/Core/WinApp.h"
#include "Engine/Core/DXCommon.h"
#include "Engine/System/Managers/SRVManager.h"
#include "Engine/System/ImGui/SceneView.h"
// c++
#include <cassert>


///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
ImGuiManager::ImGuiManager() = default;
ImGuiManager::~ImGuiManager() = default;

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void ImGuiManager::Initialize(WinApp* winApp, DXCommon* dxCommon, SRVManager* srvManager) {

	assert(winApp);
	assert(dxCommon);
	assert(srvManager);

	winApp_ = winApp;
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

#ifdef USE_IMGUI
	IMGUI_CHECKVERSION(); //　バージョンチェック
	ImGui::CreateContext(); // コンテキストの生成
	ImGui::StyleColorsDark(); // スタイルの設定
	ImGui_ImplWin32_Init(winApp_->GetHwnd()); // Win32用の初期化 
	ImGui_ImplDX12_Init( // DirectX12用の初期化
		dxCommon_->GetDevice(),
		static_cast<int>(dxCommon_->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvManager_->GetDescriptorHeap(),
		srvManager_->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		srvManager_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // ドッキングを有効にする
#endif // USE_IMGUI
}


///-------------------------------------------/// 
/// 終了
///-------------------------------------------///
void ImGuiManager::Finalize() {
#ifdef USE_IMGUI
	// ImGuiの終了処理.。
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // USE_IMGUI
}


///-------------------------------------------/// 
/// 開始処理
///-------------------------------------------///
void ImGuiManager::Begin() {
#ifdef USE_IMGUI
	// フレームの先頭でImGuiに、ここからフレームが始まる旨を告げる
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// 開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の初期に置き換える
	//ImGui::ShowDemoWindow();

	// Dock付きUI描画
	//DrawMainDockWindow();
	// ドッキング設定
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

#endif // USE_IMGUI
}


///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void ImGuiManager::End() {
#ifdef USE_IMGUI
	// 描画用のDescriptorHeapの設定
	ComPtr<ID3D12GraphicsCommandList> commandList = dxCommon_->GetCommandList();
	// ImGuiの内部コマンドを生成する
	ImGui::Render();
	// SRVの設定
	srvManager_->PreDraw();
	// 実際のCommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
#endif // USE_IMGUI
}


///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void ImGuiManager::Draw() {

	// 各ウィンドウを描画
	if (sceneView_) {
		sceneView_->Draw();
	}
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void ImGuiManager::SetSceneView(SceneView* sceneView) { sceneView_ = sceneView; }

///-------------------------------------------/// 
/// Dock付きUI描画
///-------------------------------------------///
void ImGuiManager::DrawMainDockWindow() {
#ifdef USE_IMGUI
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("DockSpace Window", nullptr, window_flags);
	ImGui::PopStyleVar(2);

	MenuBar();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);
	ImGui::End();
#endif // USE_IMGUI

	// 各ウィンドウを描画
	if (sceneView_) sceneView_->Draw();
}

///-------------------------------------------/// 
/// メニューバーのみ
///-------------------------------------------///
void ImGuiManager::MenuBar() {
#ifdef USE_IMGUI
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("Open", "Ctrl+O");
			ImGui::MenuItem("Save", "Ctrl+S");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			ImGui::MenuItem("Undo", "Ctrl+Z");
			ImGui::MenuItem("Redo", "Ctrl+Y");
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
#endif // USE_IMGUI
}
