#include "ParticleCurve.h"
#include "Math/sMath.h"

namespace MiiEngine {
	///-------------------------------------------/// 
/// コンストラクタ（カーブ）
///-------------------------------------------///
	FloatCurve::FloatCurve() {
		keys.push_back({ 0.0f, 1.0f });
		keys.push_back({ 1.0f, 0.0f });
	}

	///-------------------------------------------/// 
	/// 時間の計算（カーブ）
	///-------------------------------------------///
	float FloatCurve::Evaluate(float time) const {
		// 例外
		if (keys.empty()) return 0.0f;
		if (keys.size() == 1) return keys[0].value;

		// tが最初のキーよりも前、または最後のキーよりも後の場合は、端の値を返す
		if (time <= keys.front().time) return keys.front().value;
		if (time >= keys.back().time) return keys.back().value;

		// tがどのキーとキーの間にあるかを探す
		for (size_t i = 0; i < keys.size() - 1; ++i) {
			if (time >= keys[i].time && time <= keys[i + 1].time) {
				// 線形補間を使用して、tに対応する値を計算
				float timeRange = keys[i + 1].time - keys[i].time;
				float factor = (time - keys[i].time) / timeRange;
				return Math::Lerp(keys[i].value, keys[i + 1].value, factor);
			}
		}
		return keys.back().value;
	}

	///-------------------------------------------/// 
	/// コンストラクタ（カラーグラデーション）
	///-------------------------------------------///
	ColorGradient::ColorGradient() {
		keys.push_back({ 0.0f, {1.0f, 1.0f, 1.0f, 1.0f} });
		keys.push_back({ 1.0f, {1.0f, 1.0f, 1.0f, 1.0f} });
	}

	///-------------------------------------------/// 
	/// 時間の計算（カラーグラデーション）
	///-------------------------------------------///
	Vector4 ColorGradient::Evaluate(float time) const {
		// 例外
		if (keys.empty()) return { 1.0f, 1.0f, 1.0f, 1.0f };
		if (keys.size() == 1) return keys[0].value;

		// tが最初のキーよりも前、または最後のキーよりも後の場合は、端の値を返す
		if (time <= keys.front().time) return keys.front().value;
		if (time >= keys.back().time) return keys.back().value;

		// tがどのキーとキーの間にあるかを探す
		for (size_t i = 0; i < keys.size() - 1; ++i) {
			if (time >= keys[i].time && time <= keys[i + 1].time) {
				float timeRange = keys[i + 1].time - keys[i].time;
				float factor = (time - keys[i].time) / timeRange;
				return Math::Lerp(keys[i].value, keys[i + 1].value, factor);
			}
		}
		return keys.back().value;
	}
}

