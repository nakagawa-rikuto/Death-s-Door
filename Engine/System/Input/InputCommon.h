#pragma once
// Engine
#include "Engine/Core/ComPtr.h"
// c++
#define DIRECTINPUT_VERSION   0x0800   // DirectInputのバージョン指定
#include <dinput.h>
#include <Windows.h>
#include <array>
#include <vector>
#include <wrl.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

/// ===前方宣言=== ///
class WinApp;

///=====================================================/// 
/// 共通入力処理
///=====================================================///
class InputCommon {
public:

	InputCommon() = default;
	~InputCommon();

	// 初期化
	void Initialize(WinApp* winApp);
	// Getter
	ComPtr<IDirectInput8> GetDirectInput();

private:

	ComPtr<IDirectInput8> directInput_; // DirectInputインスタンス
};

