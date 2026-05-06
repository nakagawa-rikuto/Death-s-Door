#pragma once
/// ===Include=== ///
// C++
#include <string>
#include <d3d12.h>
// DirectXTex
#include "DirectXTex.h"
// Data
#include "Engine/DataInfo/OffScreenData.h"

namespace MiiEngine {
	/// ===前方宣言=== ///
	struct DissolveData;
	struct RadiusBlurData;
	struct VignetteData;
	struct ShatterGlassData;
}

namespace Service {
	///=====================================================/// 
	/// PostEffect
	///=====================================================///
	class PostEffect {
	public: /// ===OffScreen=== ///

		/// <summary>
		/// OffScreenの種類を設定
		/// </summary>
		/// <param name="type">設定するオフスクリーンの種類を指定します。通常は OffScreenType 列挙型の値を渡します。</param>
		static void SetOffScreenType(MiiEngine::OffScreenType type);

		/// <summary>
		/// エフェクトのリストをクリアする
		/// </summary>
		static void ClearEffects();

		/// <summary>
		/// エフェクトを追加する
		/// </summary>
		/// <param name="type">追加するエフェクトの種類</param>
		static void AddEffect(MiiEngine::OffScreenType type);

		/// <summary>
		/// エフェクトの削除
		/// </summary>
		/// <param name="type"></param>
		static void RemoveEffect(MiiEngine::OffScreenType type);

		/// <summary>
		/// DissolveDataの設定
		/// </summary>
		/// <param name="data">設定する DissolveData 型のデータ。値渡しで受け取られ、コピーされます。</param>
		static void SetDissolveData(MiiEngine::DissolveData data);

		/// <summary>
		/// RadiusBlurDataの設定
		/// </summary>
		/// <param name="data">半径ブラー処理に使用する設定を格納した RadiusBlurData 構造体（値渡し）。</param>
		static void SetRadiusBlurData(MiiEngine::RadiusBlurData data);

		/// <summary>
		/// VignetteDataの設定
		/// </summary>
		/// <param name="data">設定するビネットデータ。VignetteData 型のインスタンスで、ビネットの強度や範囲などのパラメーターを含みます。</param>
		static void SetVignetteData(MiiEngine::VignetteData data);

		/// <summary>
		/// ShatterGlassEffectDataの設定
		/// </summary>
		/// <param name="data">設定する ShatterGlassData の値。値渡しされ、内部状態がこの値で更新される。</param>
		static void SetShatterGlassData(MiiEngine::ShatterGlassData data);

		
	};
}

