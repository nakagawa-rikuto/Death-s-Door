#pragma once
/// ===include=== ///
#include "Engine/DataInfo/BlendModeData.h"

///-------------------------------------------/// 
/// PipelineType
///-------------------------------------------///
enum PipelineType {
	// 随時追加していく
	// 前景2D
	ForGround2D,
	// 背景2D
	BackGround2D,
	// 3D
	Obj3D,
	// SkyBox
	PrimitiveSkyBox,
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

// 列挙型の全値を取得する関数
constexpr std::array<PipelineType, static_cast<size_t>(PipelineType::CountOfPipelineType)> AllPipelineTypes() {
	return { PipelineType::ForGround2D, PipelineType::BackGround2D, PipelineType::Obj3D, PipelineType::PrimitiveSkyBox, PipelineType::Particle , PipelineType::Skinning3D, Line3D,
		PipelineType::OffScreen, PipelineType::Grayscale, PipelineType::Vignette, PipelineType::Dissolve, PipelineType::BoxFilter3x3, PipelineType::BoxFilter5x5, PipelineType::RadiusBlur, PipelineType::OutLine, PipelineType::ShatterGlass };
}