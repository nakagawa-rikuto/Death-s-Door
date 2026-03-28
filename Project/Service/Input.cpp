#include "Input.h"
// c++
#include <cassert>
// Input
#include "Engine/System/Input/Keyboard.h"
#include "Engine/System/Input/Mouse.h"
#include "Engine/System/Input/Controller.h"
// Locator
#include "Locator.h"

namespace Service {
	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void Input::Update() {
		Locator::GetKeyboard()->Update();
		Locator::GetMouse()->Update();
		Locator::GetController()->Update();
	}

	///-------------------------------------------/// 
	/// キーボード処理
	///-------------------------------------------///
	// キーの押下をチェック
	bool Input::PushKey(BYTE keyNum) {
		return Locator::GetKeyboard()->PushKey(keyNum);
	}
	// キーのトリガーをチェック
	bool Input::TriggerKey(BYTE keyNum) {
		return Locator::GetKeyboard()->TriggerKey(keyNum);
	}

	///-------------------------------------------/// 
	/// マウス処理
	///-------------------------------------------///
	// マウスの押下をチェック
	bool Input::PushMouse(MouseButtonType button) {
		return Locator::GetMouse()->PushMouseButton(button);
	}
	// マウスのトリガーをチェック
	bool Input::TriggerMouse(MouseButtonType button) {
		return Locator::GetMouse()->TriggerMouseButton(button);
	}
	// マウスカーソルの位置を取得（スクリーン座標系）
	POINT Input::GetMousePosition() {
		return Locator::GetMouse()->GetMouseCursorPosition();
	}
	// マウスのX軸移動量を取得
	LONG Input::GetMouseDeltaX() {
		return Locator::GetMouse()->GetMouseDeltaX();
	}
	// マウスのY軸移動量を取得
	LONG Input::GetMouseDeltaY() {
		return Locator::GetMouse()->GetMouseDeltaY();
	}
	// マウスのスクロール量を取得
	LONG Input::GetMouseDeltaScroll() {
		return Locator::GetMouse()->GetMouseDeltaScroll();
	}

	///-------------------------------------------/// 
	/// コントローラー処理
	///-------------------------------------------///
	// コントローラースティックの取得
	bool Input::GetJoystickState(int stickNo, XINPUT_STATE& out) {
		return Locator::GetController()->GetJoystickState(stickNo, out);
	}
	bool Input::GetJoystickStatePrevious(int stickNo, XINPUT_STATE& out) {
		return Locator::GetController()->GetJoystickStatePrevious(stickNo, out);
	}
	bool Input::GetJoystickState(int stickNo, DIJOYSTATE2& out) {
		return Locator::GetController()->GetJoystickState(stickNo, out);
	}
	bool Input::GetJoystickStatePrevious(int stickNo, DIJOYSTATE2& out) {
		return Locator::GetController()->GetJoystickStatePrevious(stickNo, out);
	}
	// コントローラーの押下チェック
	bool Input::PushButton(int stickNo, ControllerButtonType button) {
		return Locator::GetController()->PushButton(stickNo, button);
	}
	bool Input::TriggerButton(int stickNo, ControllerButtonType button) {
		return Locator::GetController()->TriggerButton(stickNo, button);
	}
	bool Input::ReleaseButton(int stickNo, ControllerButtonType button) {
		return Locator::GetController()->ReleaseButton(stickNo, button);
	}
	// ボタンの押し込み量を取得
	float Input::GetTriggerValue(int stickNo, ControllerButtonType button) {
		return Locator::GetController()->GetTriggerValue(stickNo, button);
	}
	// スティックの状況を取得
	StickState Input::GetLeftStickState(int stickNo) {
		return Locator::GetController()->GetLeftStickState(stickNo);
	}
	StickState Input::GetRightStickState(int stickNo) {
		return Locator::GetController()->GetRightStickState(stickNo);
	}
	float Input::GetStickValue(int stickNo, ControllerValueType valueType) {
		return Locator::GetController()->GetStickValue(stickNo, valueType);
	}
	// スティックの前フレーム状態を取得する関数
	StickState Input::GetLeftStickStatePrevious(int stickNo) {
		return Locator::GetController()->GetLeftStickStatePrevious(stickNo);
	}
	StickState Input::GetRightStickStatePrevious(int stickNo) {
		return Locator::GetController()->GetRightStickStatePrevious(stickNo);
	}
	// スティックのはじき（ Flick ）を検出する関数
	bool Input::FlickLeftStick(int stickNo, float threshold) {
		return Locator::GetController()->FlickLeftStick(stickNo, threshold);
	}
	bool Input::FlickRightStick(int stickNo, float threshold) {
		return Locator::GetController()->FlickRightStick(stickNo, threshold);
	}
}