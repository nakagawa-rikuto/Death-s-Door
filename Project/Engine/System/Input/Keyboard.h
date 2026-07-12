#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Core/ComPtr.h"
#include "Engine/DataInfo/InputData.h"
#include "Math/Vector2.h"

namespace MiiEngine {
	/// ===前方宣言=== ///
	class WinApp;

	///-------------------------------------------///
	/// キーボード
	///-------------------------------------------///
	class Keyboard {
	public:

		Keyboard() = default;
		~Keyboard();

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="winApp">Class WinApp</param>
		/// <param name="input">DirectInput</param>
		void Initialize(WinApp* winApp, IDirectInput8* input);

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();

		/// <summary>
		/// キーの押下をチェック
		/// </summary>
		/// <param name="keyNum">キーのタイプ</param>
		/// <returns>フラグを返す</returns>
		bool PushKey(BYTE keyNum);

		/// <summary>
		/// キーのトリガーをチェック
		/// </summary>
		/// <param name="keyNum">キーのタイプ</param>
		/// <returns>フラグを返す</returns>
		bool TriggerKey(BYTE keyNum);

		/// <summary>
		/// キーのリリースをチェック
		/// </summary>
		/// <param name="keyNum">キーのタイプ</param>
		/// <returns>フラグを返す</returns>
		bool ReleaseKey(BYTE keyNum);

		/// <summary>
		/// いずれかのキー入力があるかどうかをチェック
		/// </summary>
		/// <returns>フラグを返す</returns>
		bool HasAnyKeyInput() const;

		/// <summary>
		/// WASDキーの入力値を取得
		/// </summary>
		/// <returns></returns>
		Vector2 GetKeybordWASDValue();

		/// <summary>
		/// 矢印キーの入力値を取得
		/// </summary>
		/// <returns></returns>
		Vector2 GetKeybordArrowValue();

	private:
		// キーボード用のDirectInputデバイス
		ComPtr<IDirectInputDevice8> keyboard_;
		// 現在の全キーの状態
		BYTE key_[256] = {};
		// 前回の全キーの状態
		BYTE preKey_[256] = {};
	};
}

