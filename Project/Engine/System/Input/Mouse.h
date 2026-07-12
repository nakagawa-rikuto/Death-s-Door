#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Core/ComPtr.h"
#include "Engine/DataInfo/InputData.h"

namespace MiiEngine {
	/// ===前方宣言=== ///
	class WinApp;

	///=====================================================/// 
	/// マウス
	///=====================================================///
	class Mouse {
	public:

		Mouse() = default;
		~Mouse();

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="winApp">class WinApp</param>
		/// <param name="input">DirectInput</param>
		void Initialize(WinApp* winApp, IDirectInput8* input);

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();

		/// <summary>
		/// マウスボタンの押下をチェック
		/// </summary>
		/// <param name="button">マウスボタンの場所</param>
		/// <returns>フラグを返す</returns>
		bool PushMouseButton(MouseButtonType button) const;

		/// <summary>
		/// マウスボタンのトリガーをチェック
		/// </summary>
		/// <param name="button">マウスボタンの種類</param>
		/// <returns>フラグを返す</returns>
		bool TriggerMouseButton(MouseButtonType button) const;

		/// <summary>
		/// マウスボタンのリリースをチェック
		/// </summary>
		/// <param name="button">マウスボタンの種類</param>
		/// <returns>フラグを返す</returns>
		bool ReleaseMouseButton(MouseButtonType button) const;

		/// <summary>
		/// マウスカーソルの位置を取得（スクリーン座標系）
		/// </summary>
		/// <returns>座標を返す。</returns>
		POINT GetMouseCursorPosition() const;

		/// <summary>
		/// マウスのx軸を移動量を取得
		/// </summary>
		/// <returns>移動量を返す</returns>
		LONG GetMouseDeltaX() const;

		/// <summary>
		/// マウスのY軸を移動量を取得
		/// </summary>
		/// <returns>移動量を返す</returns>
		LONG GetMouseDeltaY() const;

		/// <summary>
		/// マウスのスクロール量を取得
		/// </summary>
		/// <returns>移動量を返す</returns>
		LONG GetMouseDeltaScroll() const;

	private:
		WinApp* winApp_ = nullptr;

		/*NOTE : 今回はDIMOUSESTATEを使用しているがマウスのボタンの数が４つじゃなりないのならDIMOUSESTATE2を使用すると良い*/
		// マウス用のDirectInputデバイス
		ComPtr<IDirectInputDevice8> mouse_;
		// 現在のマウスの状態
		DIMOUSESTATE mouseState_ = {};
		// 前フレームのマウスの状態
		DIMOUSESTATE preMouseState_ = {};
	};
}

