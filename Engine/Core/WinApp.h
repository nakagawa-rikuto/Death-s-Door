#pragma once
// Windows
#include <Windows.h>

// int32_tを使うためのもの
#include <cstdint>

class WinApp {

public:
	// ウィンドウサイズ
	static const int kWindowWidth = 1280; // 横幅
	static const int kWindowHeight = 720; // 縦幅

	// ウィンドウクラス名
	static const wchar_t kWindowClassName[];

public:

	/// <summary>
	/// ウィンドウの横幅の取得
	/// </summary>
	/// <returns></returns>
	const int GetWindowWidth();
	
	/// <summary>
	/// ウィンドウの縦幅の取得
	/// </summary>
	/// <returns></returns>
	const int GetWindowHeight();

	/// <summary>
	/// ウィンドウプロージャ
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="msg"></param>
	/// <param name="wparam"></param>
	/// <param name="lparam"></param>
	/// <returns></returns>
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// ゲームウィンドウの作成関数
	/// </summary>
	/// <param name="kWindowClassName"></param>
	/// <param name="kClientWidth"></param>
	/// <param name="kClientHeight"></param>
	void CreateGameWindow(
		const wchar_t* title, int32_t kClientWidth = kWindowWidth, int32_t kClientHeight = kWindowHeight);

	/// <summary>
	/// ゲームウィンドウの破棄
	/// </summary>
	void TerminateGameWindow();

	/// <summary>
	/// メッセージの処理
	/// </summary>
	/// <returns></returns>
	bool ProcessMessage();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd();

	/// <summary>
	/// ウィンドウclassの取得
	/// </summary>
	/// <returns></returns>
	WNDCLASS GetWNDClass();

private:// メンバ変数

	WNDCLASS wndClass_{}; // ウィンドウクラス
	HWND hwnd_ = nullptr; // ウィンドウハンドル
	UINT windowStyle_; // 
	bool isFullScreen_ = false; // 
	RECT windowRect_; // 
};