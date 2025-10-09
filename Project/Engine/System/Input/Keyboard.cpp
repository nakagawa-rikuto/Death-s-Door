#include "Keyboard.h"
// Engine
#include "Engine/Core/WinApp.h"
#include "InputCommon.h"
// c++
#include <cassert>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
Keyboard::~Keyboard() {
	if (keyboard_) {
		keyboard_->Unacquire();
	}
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Keyboard::Initialize(WinApp * winApp, IDirectInput8 * input) {
	HRESULT hr;

	// キーボードデバイスの生成
	hr = input->CreateDevice(GUID_SysKeyboard, &keyboard_, nullptr);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	// 排他的制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Keyboard::Update() {
	HRESULT hr;

	// 前回の入力を保持
	memcpy(preKey_, key_, sizeof(key_));

	// 情報の取得を開始
	hr = keyboard_->Acquire();
	if (SUCCEEDED(hr)) {
		keyboard_->GetDeviceState(sizeof(key_), key_);
	}

	// 全キーの入力情報を取得
	hr = keyboard_->GetDeviceState(sizeof(key_), key_);
}

///-------------------------------------------/// 
/// キーの押下をチェック
///-------------------------------------------///
bool Keyboard::PushKey(BYTE keyNum) {
	// 指定キーを押して入ればTrueを返す
	if (key_[keyNum]) {
		return true;
	}
	// そうでなければFalseを返す
	return false;
}

///-------------------------------------------/// 
/// キーのトリガーをチェック
///-------------------------------------------///
bool Keyboard::TriggerKey(BYTE keyNum) {
	// 指定キーがトリガーされていればtrueを返す
	if (key_[keyNum] && !preKey_[keyNum]) {
		return true;
	}
	// そうでなければFalseを返す
	return false;
}
