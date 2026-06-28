#include "InputTracker.h"

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// アクティブなデバイスを取得
DeviceType MiiEngine::InputTracker::GetActiveDevice() const { return activeDevice_; }
// デバイスが変更されたかどうかを取得
bool MiiEngine::InputTracker::IsDeviceChanged() const { return isChanged_; }

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void MiiEngine::InputTracker::Update(const Keyboard& keyboard, const Mouse& mouse, const Controller& controller) {
	// デバイスの状態を保存
	preDevice_ = activeDevice_;

	// コントローラーを優先して判定
	if (HasControllerInput(controller)) {
		activeDevice_ = DeviceType::Controller;
	} else if (mouse.GetMouseDeltaX() != 0 || mouse.GetMouseDeltaY() != 0 || mouse.PushMouseButton(MouseButtonType::Left) || mouse.PushMouseButton(MouseButtonType::Right) || keyboard.HasAnyKeyInput()) {
		activeDevice_ = DeviceType::Keyboard;
	}

	isChanged_ = (activeDevice_ != preDevice_);
}

///-------------------------------------------/// 
/// コントローラー入力があるかどうかを判定
///-------------------------------------------///
bool MiiEngine::InputTracker::HasControllerInput(const Controller & controller) const {
    for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
        XINPUT_STATE state{};
        if (!controller.GetJoystickState(i, state)) continue;

        // ボタン入力チェック
        if (state.Gamepad.wButtons != 0) return true;

        // スティック入力チェック（デッドゾーン考慮）
        auto leftX = std::abs(state.Gamepad.sThumbLX / 32768.0f);
        auto leftY = std::abs(state.Gamepad.sThumbLY / 32768.0f);
        auto rightX = std::abs(state.Gamepad.sThumbRX / 32768.0f);
        auto rightY = std::abs(state.Gamepad.sThumbRY / 32768.0f);
        if (leftX > 0.15f || leftY > 0.15f || rightX > 0.15f || rightY > 0.15f) return true;

        // トリガー入力チェック
        if (state.Gamepad.bLeftTrigger > 30 || state.Gamepad.bRightTrigger > 30) return true;
    }
    return false;
}
