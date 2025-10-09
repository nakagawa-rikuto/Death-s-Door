#pragma once
/// ===Includ=== ///
// Engine
#include "Engine/Core/ComPtr.h"
#include "Engine/DataInfo/InputData.h"

/// ===前方宣言=== ///
class WinApp;

///-------------------------------------------///
/// キーボード
///-------------------------------------------///
class Keyboard {
public:

	Keyboard() = default;
	~Keyboard();

	// 初期化
	void Initialize(WinApp* winApp, IDirectInput8* input);
	// 更新
	void Update();

	// キーの押下をチェック
	bool PushKey(BYTE keyNum);
	// キーのトリガーをチェック
	bool TriggerKey(BYTE keyNum);

private:
	// キーボード用のDirectInputデバイス
	ComPtr<IDirectInputDevice8> keyboard_; 
	// 現在の全キーの状態
	BYTE key_[256] = {}; 
	// 前回の全キーの状態
	BYTE preKey_[256] = {}; 
};

