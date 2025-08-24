#include "WinApp.h"
#include <string>

#ifdef _DEBUG
#include <imGui.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // _DEBUG

// ウィンドウクラスの名前
const wchar_t WinApp::kWindowClassName[] = L"DirectX_MyEngine_WindowClass";

///-------------------------------------------/// 
/// ウィンドウの横幅の取得
///-------------------------------------------///
const int WinApp::GetWindowWidth() { return kWindowWidth; }

///-------------------------------------------/// 
/// ウィンドウの縦幅の取得
///-------------------------------------------///
const int WinApp::GetWindowHeight() { return kWindowHeight; }

///-------------------------------------------/// 
/// ウィンドウプロージャ
///-------------------------------------------///
LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
#endif // _DEBUG

	// メッセージに応じたゲーム固有の処理
	switch (msg) {

		// ウィンドウが破棄された
	case WM_DESTROY:

		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

///-------------------------------------------/// 
/// ゲームウィンドウの作成
///-------------------------------------------///
void WinApp::CreateGameWindow(const wchar_t* title, int32_t kClientWidth, int32_t kClientHeight) {

	/* /////////////////////////////////////
					COMの初期化
	*/ /////////////////////////////////////
	CoInitializeEx(0, COINIT_MULTITHREADED);

	/* /////////////////////////////////////
				ウィンドウクラスの登録
	*/ /////////////////////////////////////
	// ウィンドウプロージャ
	wndClass_.lpfnWndProc = WindowProc;

	// ウィンドウクラス名
	wndClass_.lpszClassName = kWindowClassName;

	// インスタンスハンドル
	wndClass_.hInstance = GetModuleHandle(nullptr);

	// カーソル
	wndClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録
	RegisterClass(&wndClass_);

	/* /////////////////////////////////////
			　　　　ウィンドの作成
	*/ /////////////////////////////////////
	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0, 0, kClientWidth, kClientHeight };

	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	hwnd_ = CreateWindow(
		wndClass_.lpszClassName,    // 利用するクラス名
		title,                      // タイトルバーの文字
		WS_OVERLAPPEDWINDOW,        // ウィンドウスタイル
		CW_USEDEFAULT,              // 表示X座標
		CW_USEDEFAULT,              // 表示Y座標
		wrc.right - wrc.left,       // ウィンドウの横幅
		wrc.bottom - wrc.top,       // ウィンドウの縦幅
		nullptr,                    // 親ウィンドウハンドル
		nullptr,                    // メニューハンドル
		wndClass_.hInstance,        // インスタンスハンドル
		nullptr);                   // オプション

	// ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);
}

///-------------------------------------------/// 
/// ゲームウィンドウの破棄
///-------------------------------------------///
void WinApp::TerminateGameWindow() {
	// ウィンドウクラスを登録解除
	UnregisterClass(wndClass_.lpszClassName, wndClass_.hInstance);
}

///-------------------------------------------/// 
/// メッセージの処理
///-------------------------------------------///
bool WinApp::ProcessMessage() {
	MSG msg{}; // メッセージ

	// Windowにメッセージが来てたら最優先で処理させる
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg); // キー入力メッセージの処理
		DispatchMessage(&msg); // ウィンドウプロシージャにメッセージを送る
	}

	if (msg.message == WM_QUIT) { //　終了メッセージが来たらループを抜ける
		return true;
	}

	return false;
}

/// <summary>
/// ウィンドウハンドルの取得
/// </summary>
HWND WinApp::GetHwnd() { return hwnd_; }

/// <summary>
/// ウィンドウクラスの取得
/// </summary>
/// <returns></returns>
WNDCLASS WinApp::GetWNDClass() { return wndClass_; }
