#pragma once
/// ===Includ=== ///
#include "Engine/DataInfo/InputData.h"

///=====================================================/// 
/// InputService
///=====================================================///
class InputService {
public: // メンバ関数
	// 更新
	static void Update();

	/// ===キーボード=== ///
	// キーの押下をチェック
	static bool PushKey(BYTE keyNum);
	// キーのトリガーをチェック
	static bool TriggerKey(BYTE keyNum);

	/// ===マウス=== ///
	// マウスボタンの押下をチェック
	static bool PushMouse(MouseButtonType button);
	// マウスボタンのトリガーをチェック
	static bool TriggerMouse(MouseButtonType button);
	// マウスカーソルの位置を取得（スクリーン座標系）
	static POINT GetMousePosition();
	// マウスのx軸を移動量を取得
	static LONG GetMouseDeltaX();
	// マウスのY軸を移動量を取得
	static LONG GetMouseDeltaY();
	// マウスのスクロール量を取得
	static LONG GetMouseDeltaScroll();

	/// ===コントローラ=== ///
	// コントローラースティックの取得
	static bool GetJoystickState(int stickNo, XINPUT_STATE& out);
	static bool GetJoystickStatePrevious(int stickNo, XINPUT_STATE& out);
	static bool GetJoystickState(int stickNo, DIJOYSTATE2& out);
	static bool GetJoystickStatePrevious(int stickNo, DIJOYSTATE2& out);

	// コントローラーの押下チェック
	static bool PushButton(int stickNo, ControllerButtonType button);
	static bool TriggerButton(int stickNo, ControllerButtonType button);
	static bool ReleaseButton(int stickNo, ControllerButtonType button);

	// ボタンの押し込み量を取得
	static float GetTriggerValue(int stickNo, ControllerButtonType button);

	// スティックの状況を取得
	static StickState GetLeftStickState(int stickNo);
	static StickState GetRightStickState(int stickNo);
	static float GetStickValue(int stickNo, ControllerValueType valueType);
	// スティックの前フレーム状態を取得する関数
	static StickState GetLeftStickStatePrevious(int stickNo);
	static StickState GetRightStickStatePrevious(int stickNo);
	// スティックのはじき（ Flick ）を検出する関数
	static bool FlickLeftStick(int stickNo, float threshold = 0.7f);
	static bool FlickRightStick(int stickNo, float threshold = 0.7f);

};

