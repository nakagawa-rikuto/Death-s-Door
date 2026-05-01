#pragma once
/// ===Include=== ///
// C++
#include <vector>
#include <algorithm>
// Math
#include "Math/Vector4.h"

namespace MiiEngine {
	// 一つのキーフレーム
	template<typename T>
	struct Keyframe {
		float time; // 時間
		T value;    // 値
	};


	///=====================================================///		
	///  カーブクラス
	///=====================================================///
	class FloatCurve {
	public:

		std::vector<Keyframe<float>> keys;

		FloatCurve();

		/// <summary>
		/// 時間の計算
		/// </summary>
		float Evaluate(float time) const;
	};
 

	///=====================================================/// 
	/// カラーグラデーションクラス 
	///=====================================================///
	class ColorGradient {
	public:
		std::vector<Keyframe<Vector4>> keys;

		ColorGradient();

		/// <summary>
		/// 時間の計算
		/// </summary>
		Vector4 Evaluate(float time) const;
	};
}

