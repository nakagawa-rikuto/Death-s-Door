#include "InputCommon.h"
// Engine
#include "Engine/Core/WinApp.h"
// c++
#include <cassert>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
InputCommon::~InputCommon() { 
	directInput_.Reset(); 
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void InputCommon::Initialize(WinApp * winApp) {
	HRESULT hr;

	// DirectInputのインスタンスの生成
	hr = DirectInput8Create(winApp->GetWNDClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ComPtr<IDirectInput8> InputCommon::GetDirectInput() { return directInput_; }
