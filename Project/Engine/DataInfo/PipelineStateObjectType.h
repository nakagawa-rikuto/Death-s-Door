#pragma once
/// ===include=== ///
#include "Engine/DataInfo/BlendModeData.h"

namespace MiiEngine {
	///-------------------------------------------/// 
	/// PipelineType
	///-------------------------------------------///
	enum PipelineType {
		// 前景2D
		ForGround2D,
		// 背景2D
		BackGround2D,
		// CircularGauge(Sprite)
		CircularGauge2D,
		// 3D
		Obj3D,
		// SkyBox
		PrimitiveSkyBox,
		// Ocean
		Ocean,
		// FFTOcean
		FFTOcean,
		// Particle
		Particle,
		// Skinning3D
		Skinning3D,
		// Line3D
		Line3D,
		// OffScreen
		OffScreen,
		// Grayscale
		Grayscale,
		// Vignette
		Vignette,
		// Dissolve
		Dissolve,
		// BoxFilter3x3
		BoxFilter3x3,
		// BoxFilter5x5
		BoxFilter5x5,
		// RadiusBlur
		RadiusBlur,
		// OutLine
		OutLine,
		// ShatterGlass
		ShatterGlass,

		// 総数(これは最後にしなければいけない)
		CountOfPipelineType,
	};

	///-------------------------------------------/// 
	/// CSPipelineType
	///-------------------------------------------///
	enum class CSPipelineType {
		// Ocean
		Ocean,
		// FFTOcean
		FFTOcean,
		// Ripples
		Ripples,

		// 総数(これは最後にしなければいけない)
		CountOfCSPipelineType,
	};


	// 列挙型の全値を取得する関数
	constexpr std::array<PipelineType, static_cast<size_t>(PipelineType::CountOfPipelineType)> AllPipelineTypes() {
		return {
			// Graphics Pipelines
			PipelineType::ForGround2D,
			PipelineType::BackGround2D,
			PipelineType::CircularGauge2D,
			PipelineType::Obj3D,
			PipelineType::PrimitiveSkyBox,
			PipelineType::Particle ,
			PipelineType::Skinning3D,
			PipelineType::Ocean,
			PipelineType::FFTOcean,
			PipelineType::Line3D,
			PipelineType::OffScreen,
			PipelineType::Grayscale,
			PipelineType::Vignette,
			PipelineType::Dissolve,
			PipelineType::BoxFilter3x3,
			PipelineType::BoxFilter5x5,
			PipelineType::RadiusBlur,
			PipelineType::OutLine,
			PipelineType::ShatterGlass,
		};
	}

	// 列挙型の全値を取得する関数
	constexpr std::array<CSPipelineType, static_cast<size_t>(CSPipelineType::CountOfCSPipelineType)> AllCSPipelineTypes() {
		return {
			CSPipelineType::Ocean,
			CSPipelineType::FFTOcean,
			CSPipelineType::Ripples,
		};
	}
}