#include "InputService.h"
// c++
#include <cassert>
// Input
#include "Engine/System/Input/Keyboard.h"
#include "Engine/System/Input/Mouse.h"
#include "Engine/System/Input/Controller.h"
// ServiceLocator
#include "ServiceLocator.h"

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void InputService::Update() {
	ServiceLocator::GetKeyboard()->Update();
	ServiceLocator::GetMouse()->Update();
	ServiceLocator::GetController()->Update();
}

///-------------------------------------------/// 
/// キーボード処理
///-------------------------------------------///
// キーの押下をチェック
bool InputService::PushKey(BYTE keyNum) {
	return ServiceLocator::GetKeyboard()->PushKey(keyNum);
}
// キーのトリガーをチェック
bool InputService::TriggerKey(BYTE keyNum) {
	return ServiceLocator::GetKeyboard()->TriggerKey(keyNum);
}

///-------------------------------------------/// 
/// マウス処理
///-------------------------------------------///
// マウスの押下をチェック
bool InputService::PushMouse(MouseButtonType button) {
	return ServiceLocator::GetMouse()->PushMaouseButton(button);
}
// マウスのトリガーをチェック
bool InputService::TriggerMouse(MouseButtonType button) {
	return ServiceLocator::GetMouse()->TriggerMouseButton(button);
}
// マウスカーソルの位置を取得（スクリーン座標系）
POINT InputService::GetMousePosition() {
	return ServiceLocator::GetMouse()->GetMouseCursorPosition();
}
// マウスのX軸移動量を取得
LONG InputService::GetMouseDeltaX() {
	return ServiceLocator::GetMouse()->GetMouseDeltaX();
}
// マウスのY軸移動量を取得
LONG InputService::GetMouseDeltaY() {
	return ServiceLocator::GetMouse()->GetMouseDeltaY();
}
// マウスのスクロール量を取得
LONG InputService::GetMouseDeltaScroll() {
	return ServiceLocator::GetMouse()->GetMouseDeltaScroll();
}

///-------------------------------------------/// 
/// コントローラー処理
///-------------------------------------------///
// コントローラースティックの取得
bool InputService::GetJoystickState(int stickNo, XINPUT_STATE& out) {
	return ServiceLocator::GetController()->GetJoystickState(stickNo, out);
}
bool InputService::GetJoystickStatePrevious(int stickNo, XINPUT_STATE& out) {
	return ServiceLocator::GetController()->GetJoystickStatePrevious(stickNo, out);
}
bool InputService::GetJoystickState(int stickNo, DIJOYSTATE2& out) {
	return ServiceLocator::GetController()->GetJoystickState(stickNo, out);
}
bool InputService::GetJoystickStatePrevious(int stickNo, DIJOYSTATE2& out) {
	return ServiceLocator::GetController()->GetJoystickStatePrevious(stickNo, out);
}
// コントローラーの押下チェック
bool InputService::PushButton(int stickNo, ControllerButtonType button) {
	return ServiceLocator::GetController()->PushButton(stickNo, button);
}
bool InputService::TriggerButton(int stickNo, ControllerButtonType button) {
	return ServiceLocator::GetController()->TriggerButton(stickNo, button);
}
bool InputService::ReleaseButton(int stickNo, ControllerButtonType button) {
	return ServiceLocator::GetController()->ReleaseButton(stickNo, button);
}
// ボタンの押し込み量を取得
float InputService::GetTriggerValue(int stickNo, ControllerButtonType button) {
	return ServiceLocator::GetController()->GetTriggerValue(stickNo, button);
}
// スティックの状況を取得
StickState InputService::GetLeftStickState(int stickNo) {
	return ServiceLocator::GetController()->GetLeftStickState(stickNo);
}
StickState InputService::GetRightStickState(int stickNo) {
	return ServiceLocator::GetController()->GetRightStickState(stickNo);
}
float InputService::GetStickValue(int stickNo, ControllerValueType valueType) {
	return ServiceLocator::GetController()->GetStickValue(stickNo, valueType);
}
// スティックの前フレーム状態を取得する関数
StickState InputService::GetLeftStickStatePrevious(int stickNo) {
	return ServiceLocator::GetController()->GetLeftStickStatePrevious(0);
}
StickState InputService::GetRightStickStatePrevious(int stickNo) {
	return ServiceLocator::GetController()->GetRightStickStatePrevious(0);
}
// スティックのはじき（ Flick ）を検出する関数
bool InputService::FlickLeftStick(int stickNo, float threshold) {
	return ServiceLocator::GetController()->FlickLeftStick(0, threshold);
}
bool InputService::FlickRightStick(int stickNo, float threshold) {
	return ServiceLocator::GetController()->FlickRightStick(0, threshold);
}
