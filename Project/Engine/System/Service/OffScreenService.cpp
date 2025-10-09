#include "OffScreenService.h"
// c++
#include <cassert>
// Engine
#include "Engine/Graphics/OffScreen/OffScreenRenderer.h"
// ServiceLocator
#include "ServiceLocator.h"

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// OffScreenTypeの設定
void OffScreenService::SetOffScreenType(OffScreenType type) {
	return ServiceLocator::GetOffScreenRenderer()->SetType(type);
}
