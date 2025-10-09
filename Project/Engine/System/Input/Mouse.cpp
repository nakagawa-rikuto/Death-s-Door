#include "Mouse.h"
// Engine
#include "Engine/Core/WinApp.h"
#include "InputCommon.h"
// c++
#include <cassert>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
Mouse::~Mouse() {
	if(mouse_) {
		mouse_->Unacquire();
	}
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Mouse::Initialize(WinApp * winApp, IDirectInput8 * input) {
	HRESULT hr;
	winApp_ = winApp;

	// マウスデバイスの生成
	hr = input->CreateDevice(GUID_SysMouse, &mouse_, nullptr);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(hr));
	// 排他的制御レベルのセット
	hr = mouse_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(hr));
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Mouse::Update() {
	HRESULT hr;

	// 前回の入力を保持
	preMouseState_ = mouseState_;

	// 状態を取得
	hr = mouse_->Acquire();
	if (SUCCEEDED(hr)) {
		mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);
	}
}

///-------------------------------------------/// 
/// マウスの押下チェック
///-------------------------------------------///
bool Mouse::PushMaouseButton(MouseButtonType button) {
	// マウスボタンが押されている場合にtrueを返す
	return (mouseState_.rgbButtons[static_cast<int>(button)] & 0x80) != 0;
}

///-------------------------------------------/// 
/// マウスのトリガーチェック
///-------------------------------------------///
bool Mouse::TriggerMouseButton(MouseButtonType button) {
	// マウスボタンがトリガー（押された瞬間）の場合にtrueを返す
	return (mouseState_.rgbButtons[static_cast<int>(button)] & 0x80) &&
		!(preMouseState_.rgbButtons[static_cast<int>(button)] & 0x80);
}

///-------------------------------------------/// 
/// マウスカーソルの位置を取得（スクリーン座標系）
///-------------------------------------------///
POINT Mouse::GetMouseCursorPosition() const {
	POINT cursorPosition{};
	if (GetCursorPos(&cursorPosition)) {
		// スクリーン座標からクライアント座標へ変換
		if (ScreenToClient(winApp_->GetHwnd(), &cursorPosition)) {
			return cursorPosition; // マウスカーソルのクライアント座標を返す
		}
	}
	return { -1, -1 }; // 失敗した場合のデフォルト値
}

///-------------------------------------------/// 
/// マウスのX軸移動量を取得
///-------------------------------------------///
LONG Mouse::GetMouseDeltaX() const {
	// マウスのX軸の移動量を返す
	return mouseState_.lX;
}

///-------------------------------------------/// 
/// マウスのY軸移動量を取得
///-------------------------------------------///
LONG Mouse::GetMouseDeltaY() const {
	// マウスのY軸の移動量を返す
	return mouseState_.lY;
}

///-------------------------------------------/// 
/// マウスのスクロール量を取得
///-------------------------------------------///
LONG Mouse::GetMouseDeltaScroll() const {
	// マウスのスクロール量を返す
	return mouseState_.lZ;
}
