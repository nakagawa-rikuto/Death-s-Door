#pragma once
/// ===Includ=== ///
// Engine
#include "Engine/Core/ComPtr.h"
#include "Engine/DataInfo/InputData.h"

/// ===前方宣言=== ///
class WinApp;

///=====================================================/// 
/// マウス
///=====================================================///
class Mouse {
public:

	Mouse() = default;
	~Mouse();

	// 初期化
	void Initialize(WinApp* winApp, IDirectInput8* input);
	// 更新
	void Update();

	// マウスボタンの押下をチェック
	bool PushMaouseButton(MouseButtonType button);
	// マウスボタンのトリガーをチェック
	bool TriggerMouseButton(MouseButtonType button);
	// マウスカーソルの位置を取得（スクリーン座標系）
	POINT GetMouseCursorPosition() const;
	// マウスのx軸を移動量を取得
	LONG GetMouseDeltaX() const;
	// マウスのY軸を移動量を取得
	LONG GetMouseDeltaY() const;
	// マウスのスクロール量を取得
	LONG GetMouseDeltaScroll() const;

private:
	WinApp* winApp_ = nullptr;

	/*NOTE : 今回はDIMOUSESTATEを使用しているがマウスのボタンの数が４つじゃなりないのなら
	         DIMOUSESTATE2を使用すると良い*/
	// マウス用のDirectInputデバイス
	ComPtr<IDirectInputDevice8> mouse_; 
	// 現在のマウスの状態
	DIMOUSESTATE mouseState_ = {}; 
	// 前フレームのマウスの状態
	DIMOUSESTATE preMouseState_ = {}; 
	
};

