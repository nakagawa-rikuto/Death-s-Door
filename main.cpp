/// ===include=== ///
#include "Engine/Scene/Framework.h"
#include "Engine/Scene/MyGame.h"

/// ===ゲームタイトル=== ///
const wchar_t kWindowTitle[] = L"MiiEngine";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// 宣言
	Framework* game = new MyGame();
	// 実行
	game->Run(kWindowTitle);
	// 解放
	delete game;

	return 0;
}