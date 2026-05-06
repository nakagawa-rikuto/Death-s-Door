#pragma once
namespace MiiEngine {
	/// ===タイプ=== ///
	enum class OffScreenType {
		CopyImage,
		Grayscale,
		Vignette,
		BoxFilter3x3,
		BoxFilter5x5,
		RadiusBlur,
		OutLine,
		Dissolve,
		ShatterGlass,
		Bloom,
	};
}