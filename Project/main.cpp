/// ===include=== ///
#include "Engine/Scene/Framework.h"
#include "Engine/Scene/MyGame.h"
#include <windows.h>

/// ===ゲームタイトル=== ///
const wchar_t kWindowTitle[] = L"KaijounoKessen";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// 宣言
	std::unique_ptr<MiiEngine::Framework> game = std::make_unique<MiiEngine::MyGame>();
	// 実行
	game->Run(kWindowTitle);
	// 解放
	game.reset();

	return 0;
}