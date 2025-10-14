#pragma once
/// ===Include=== ///
// C++
#include <string>
#include <d3d12.h>
// DirectXTex
#include "DirectXTex.h"
// Data
#include "Engine/DataInfo/OffScreenData.h"

/// ===前方宣言=== ///
struct DissolveData;
struct RadiusBlurData;
struct VignetteData;
struct ShatterGlassData;


///=====================================================/// 
/// OffScreenService
///=====================================================///
class OffScreenService {
public: /// ===OffScreen=== ///
	static void SetOffScreenType(OffScreenType type);

	// Dissolvedata
	static void SetDissolveData(DissolveData data);

	// RadiusBlurData
	static void SetRadiusBlurData(RadiusBlurData data);

	// VignetteData
	static void SetVignetteData(VignetteData data);

	// ShatterGlassEffectData
	static void SetShatterGlassData(ShatterGlassData data);

};

