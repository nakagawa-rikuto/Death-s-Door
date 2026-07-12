#pragma once
/// ===Include=== ///
#include "Engine/DataInfo/InputData.h"
#include "Math/Vector2.h"

namespace Service {
	///=====================================================/// 
	/// Input
	///=====================================================///
	class Input {
	public: // メンバ関数

		/// ===キーボード=== ///

		/// <summary>
		///  キーの押下をチェック
		/// </summary>
		/// <param name="keyNum">プッシュするキーを表す BYTE 型の番号。</param>
		/// <returns>操作が成功した場合に true を、失敗した場合に false を返します。</returns>
		static bool PushKey(BYTE keyNum);

		/// <summary>
		/// キーのトリガーをチェック
		/// </summary>
		/// <param name="keyNum">トリガーするキーを識別する BYTE 型の番号。</param>
		/// <returns>操作が成功した場合は true、失敗した場合は false を返します。</returns>
		static bool TriggerKey(BYTE keyNum);

		/// <summary>
		/// キーのリリースをチェック
		/// </summary>
		/// <param name="keyNum">リリースするキーを識別する BYTE 型の番号。</param>
		/// <returns>操作が成功した場合は true、失敗した場合は false を返します。</returns>
		static bool ReleaseKey(BYTE keyNum);

		/// <summary>
		/// いずれかのキー入力があるかどうかをチェック
		/// </summary>
		/// <returns></returns>
		static bool HasAnyKeyInput();

		/// <summary>
		/// WASDキーの入力値を取得
		/// </summary>
		/// <returns></returns>
		static Vector2 GetKeybordWASDValue();

		/// <summary>
		/// 矢印キーの入力値を取得
		/// </summary>
		/// <returns></returns>
		static Vector2 GetKeybordArrowValue();

		/// ===マウス=== ///

		/// <summary>
		/// マウスボタンの押下をチェック
		/// </summary>
		/// <param name="button">押すマウスボタンを指定します（MouseButtonType）。</param>
		/// <returns>操作が成功した場合は true、失敗した場合は false を返します。</returns>
		static bool PushMouse(MouseButtonType button);

		/// <summary>
		/// マウスボタンのトリガーをチェック
		/// </summary>
		/// <param name="button">トリガーするマウスボタンを示す MouseButtonType 型の値。</param>
		/// <returns>操作が成功した場合は true、失敗した場合は false を返します。</returns>
		static bool TriggerMouse(MouseButtonType button);

		/// <summary>
		/// マウスボタンのリリースをチェック
		/// </summary>
		/// <param name="button">リリースするマウスボタンを示す MouseButtonType 型の値。</param>
		/// <returns>操作が成功した場合は true、失敗した場合は false を返します。</returns>
		static bool ReleaseMouse(MouseButtonType button);

		/// <summary>
		/// マウスカーソルの位置を取得（スクリーン座標系）
		/// </summary>
		/// <returns>現在のマウス位置を示すPOINT構造体を返します。</returns>
		static POINT GetMousePosition();

		/// <summary>
		/// マウスのx軸を移動量を取得
		/// </summary>
		/// <returns>X 軸方向の移動量を表す LONG 値。正の値は右方向の移動、負の値は左方向の移動を示します。</returns>
		static LONG GetMouseDeltaX();

		/// <summary>
		/// マウスのY軸を移動量を取得
		/// </summary>
		/// <returns>Y軸方向の移動量の差分をLONG型で返します。</returns>
		static LONG GetMouseDeltaY();

		/// <summary>
		/// マウスのスクロール量を取得
		/// </summary>
		/// <returns>マウスのスクロール量を表すLONG値。正の値は上方向/前方のスクロール、負の値は下方向/後方のスクロールを示すことが一般的です。</returns>
		static LONG GetMouseDeltaScroll();

		/// ===コントローラ=== ///

		/// <summary>
		/// コントローラースティックの取得
		/// </summary>
		/// <param name="stickNo">取得対象のジョイスティックの番号（インデックス）。</param>
		/// <param name="out">取得した状態を格納する出力パラメーター。XINPUT_STATE 型への参照。</param>
		/// <returns>状態を正常に取得できた場合は true、取得できなかった場合は false を返します。</returns>
		static bool GetJoystickState(int stickNo, XINPUT_STATE& out);
		static bool GetJoystickState(int stickNo, DIJOYSTATE2& out);

		/// <summary>
		/// コントローラースティックの前回の状態を取得
		/// </summary>
		/// <param name="stickNo">取得対象のジョイスティック番号（インデックス）。どのスティックの状態を取得するかを指定します。</param>
		/// <param name="out">出力パラメーター。前回の状態が受け取られる XINPUT_STATE 構造体への参照。関数成功時に値が設定されます。</param>
		/// <returns>前回の状態が正常に取得できた場合は true を、取得に失敗した場合は false を返します。</returns>
		static bool GetJoystickStatePrevious(int stickNo, XINPUT_STATE& out);
		static bool GetJoystickStatePrevious(int stickNo, DIJOYSTATE2& out);

		/// <summary>
		/// コントローラーの押下チェック
		/// </summary>
		/// <param name="stickNo">操作対象のコントローラ（スティック）を識別する番号。どのコントローラに対してボタン操作を行うかを指定します。</param>
		/// <param name="button">押すボタンを示す値。ControllerButtonType 列挙型のいずれかを指定します。</param>
		/// <returns>操作が成功したかどうかを示す値。成功した場合は true、失敗した場合は false。</returns>
		static bool PushButton(int stickNo, ControllerButtonType button);

		/// <summary>
		/// コントローラーのトリガーチェック
		/// </summary>
		/// <param name="stickNo">判定対象のコントローラーまたはスティックの識別番号（インデックス）。</param>
		/// <param name="button">判定するボタンを表す ControllerButtonType 型の値。</param>
		/// <returns>指定したボタンがトリガーされていれば true、そうでなければ false。</returns>
		static bool TriggerButton(int stickNo, ControllerButtonType button);

		/// <summary>
		/// コントローラーのボタンを離した状態をチェック
		/// </summary>
		/// <param name="stickNo">操作対象のスティックまたはコントローラーの番号（インデックス）です。</param>
		/// <param name="button">解除するボタンを示す ControllerButtonType 型の値です。</param>
		/// <returns>ボタンの解放処理が成功した場合は true、失敗した場合は false を返します。</returns>
		static bool ReleaseButton(int stickNo, ControllerButtonType button);

		/// <summary>
		/// ボタンの押し込み量を取得
		/// </summary>
		/// <param name="stickNo">入力を取得するコントローラのスティックを識別する番号。</param>
		/// <param name="button">取得対象のボタン（トリガー）を示す ControllerButtonType 型の値。</param>
		/// <returns>指定したスティックの指定ボタンに対応するトリガーの現在の入力値を返します。通常は 0.0（未押下）から 1.0（全押下）の範囲の浮動小数点値です。</returns>
		static float GetTriggerValue(int stickNo, ControllerButtonType button);

		/// <summary>
		/// スティックの状況を取得
		/// </summary>
		/// <param name="stickNo">状態を取得するスティックの番号を指定します。</param>
		/// <returns>指定したスティックの左スティックの状態を示す StickState 値を返します。</returns>
		static Vector2 GetLeftStickState(int stickNo);
		static Vector2 GetRightStickState(int stickNo);

		/// <summary>
		/// スティックの前フレーム状態を取得する関数
		/// </summary>
		/// <param name="stickNo">状態を取得する左スティックの番号（インデックス）。通常はコントローラや入力デバイスのスティック識別子を指定します。</param>
		/// <returns>指定したスティック番号に対応する左スティックの直前の状態を表す StickState 値。</returns>
		static Vector2 GetLeftStickStatePrevious(int stickNo);
		static Vector2 GetRightStickStatePrevious(int stickNo);

		/// <summary>
		/// コントローラーのスティックの値を取得
		/// </summary>
		/// <param name="stickNo">取得するスティックの番号（識別子）。</param>
		/// <param name="valueType">取得する値の種類を示す ControllerValueType 型の値。</param>
		/// <returns>指定したスティックと値種別に対応する浮動小数点値（float）。</returns>
		static float GetStickValue(int stickNo, ControllerValueType valueType);

		/// <summary>
		/// スティックのはじき（ Flick ）を検出する関数
		/// </summary>
		/// <param name="stickNo">判定するスティックの番号。</param>
		/// <param name="threshold">フリックと見なす入力の強さの閾値。省略時の既定値は 0.7f。</param>
		/// <returns>フリックが検出された場合は true、検出されない場合は false。</returns>
		static bool FlickLeftStick(int stickNo, float threshold = 0.7f);
		static bool FlickRightStick(int stickNo, float threshold = 0.7f);

		/// <summary>
		/// 現在アクティブな入力デバイスを取得
		/// </summary>
		/// <returns>デバイスの種類</returns>
		static DeviceType GetActiveDevice();

		/// <summary>
		/// 入力デバイスの変更をチェック
		/// </summary>
		/// <returns>デバイスが変更された場合は true、そうでない場合は false</returns>
		static bool IsDeviceChanged();
	};
}