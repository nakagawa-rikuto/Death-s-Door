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
// Dissolve
void OffScreenService::SetDissolveData(DissolveData data) {
	if (ServiceLocator::GetOffScreenRenderer()->GetType() != OffScreenType::Dissolve) return;
	return ServiceLocator::GetOffScreenRenderer()->GetDissolve()->SetData(data);
}
// RadiusBlur
void OffScreenService::SetRadiusBlurData(RadiusBlurData data) {
	if (ServiceLocator::GetOffScreenRenderer()->GetType() != OffScreenType::RadiusBlur) return;
	return ServiceLocator::GetOffScreenRenderer()->GetRadiusBlur()->SetData(data);
}
// Vignette
void OffScreenService::SetVignetteData(VignetteData data) {
	if (ServiceLocator::GetOffScreenRenderer()->GetType() != OffScreenType::Vignette) return;
	return ServiceLocator::GetOffScreenRenderer()->GetVignette()->SetData(data);
}
// ShatterGlass
void OffScreenService::SetShatterGlassData(ShatterGlassData data) {
	if (ServiceLocator::GetOffScreenRenderer()->GetType() != OffScreenType::ShatterGlass) return;
	return ServiceLocator::GetOffScreenRenderer()->GetShatterGlass()->SetData(data);
}