#pragma once
/// ===Include=== ///
#include "Controller.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace MiiEngine {
	///=====================================================/// 
	/// InputTracker
	/// どのデバイスが操作されているかを追跡するクラス
	///=====================================================///
	class InputTracker {
	public:

		/// <summary>
		/// 更新処理
		/// </summary>
		/// <param name="keyboard">キーボード</param>
		/// <param name="mouse">マウス</param>
		/// <param name="controller">コントローラ</param>
		void Update(const Keyboard& keyboard, const Mouse& mouse, const Controller& controller);

	public: /// ===Getter=== ///
		// アクティブなデバイスを取得
		DeviceType GetActiveDevice() const;
		// デバイスが変更されたかどうかを取得
		bool IsDeviceChanged() const;

	private:

		DeviceType activeDevice_ = DeviceType::None; // 現在アクティブなデバイス
		DeviceType preDevice_ = DeviceType::None;	 // 前回アクティブなデバイス
		bool isChanged_ = false;				     // デバイスが変更されたかどうかのフラグ

	private:

		/// <summary>
		/// コントローラー入力があるかどうかを判定
		/// </summary>
		/// <param name="controller">コントローラー</param>
		/// <returns></returns>
		bool HasControllerInput(const Controller& controller) const;
	};

}


