#pragma once
/// ===Include=== ///
// C++
#include <string>
#include <d3d12.h>
// DirectXTex
#include "DirectXTex.h"
// Data
#include "Engine/DataInfo/OffScreenTypeData.h"


///=====================================================/// 
/// OffScreenService
///=====================================================///
class OffScreenService {
public: /// ===OffScreen=== ///
	static void SetOffScreenType(OffScreenType type);

};

