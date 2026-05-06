#include "PostEffect.h"
// c++
#include <cassert>
// Engine
#include "Engine/Graphics/OffScreen/OffScreenRenderer.h"
// Effect
#include "Engine/Graphics/OffScreen/Effect/CopyImageEffect.h"
#include "Engine/Graphics/OffScreen/Effect/GrayscaleEffect.h"
#include "Engine/Graphics/OffScreen/Effect/VignetteEffect.h"
#include "Engine/Graphics/OffScreen/Effect/OutLineEffect.h"
#include "Engine/Graphics/OffScreen/Effect/BoxFilter3x3Effect.h"
#include "Engine/Graphics/OffScreen/Effect/BoxFilter5x5Effect.h"
#include "Engine/Graphics/OffScreen/Effect/RadiusBlurEffect.h"
#include "Engine/Graphics/OffScreen/Effect/DissolveEffect.h"
#include "Engine/Graphics/OffScreen/Effect/ShatterGlassEffect.h"
// Locator
#include "Locator.h"

namespace Service {
	///-------------------------------------------/// 
	/// Setter
	///-------------------------------------------///
	// OffScreenTypeの設定
	void PostEffect::SetOffScreenType(MiiEngine::OffScreenType type) {
		Locator::GetOffScreenRenderer()->SetType(type);
	}

	///-------------------------------------------/// 
	/// エフェクトのリストをクリアする
	///-------------------------------------------///
	void PostEffect::ClearEffects() {
		Locator::GetOffScreenRenderer()->ClearEffects();
	}

	///-------------------------------------------/// 
	/// エフェクトを追加する
	///-------------------------------------------///
	void PostEffect::AddEffect(MiiEngine::OffScreenType type) {
		Locator::GetOffScreenRenderer()->AddEffect(type);
	}

	///-------------------------------------------/// 
	/// エフェクトの削除
	///-------------------------------------------///
	void PostEffect::RemoveEffect(MiiEngine::OffScreenType type) {
		Locator::GetOffScreenRenderer()->RemoveEffect(type);
	}

	// Dissolve
	void PostEffect::SetDissolveData(MiiEngine::DissolveData data) {
		if (auto dissolve = dynamic_cast<MiiEngine::DissolveEffect*>(Locator::GetOffScreenRenderer()->GetRenderPass(MiiEngine::OffScreenType::Dissolve))) {
			dissolve->SetData(data);
		} else {
			assert(false && "DissolveEffectが見つかりませんでした。");
		}
	}
	// RadiusBlur
	void PostEffect::SetRadiusBlurData(MiiEngine::RadiusBlurData data) {
		if (auto radiusBlur = dynamic_cast<MiiEngine::RadiusBlurEffect*>(Locator::GetOffScreenRenderer()->GetRenderPass(MiiEngine::OffScreenType::RadiusBlur))) {
			radiusBlur->SetData(data);
		} else {
			assert(false && "RadiusBlurEffectが見つかりませんでした。");
		}
	}
	// Vignette
	void PostEffect::SetVignetteData(MiiEngine::VignetteData data) {
		if (auto vignette = dynamic_cast<MiiEngine::VignetteEffect*>(Locator::GetOffScreenRenderer()->GetRenderPass(MiiEngine::OffScreenType::Vignette))) {
			vignette->SetData(data);
		} else {
			assert(false && "VignetteEffectが見つかりませんでした。");
		}
	}
	// ShatterGlass
	void PostEffect::SetShatterGlassData(MiiEngine::ShatterGlassData data) {
		if (auto shatterGlass = dynamic_cast<MiiEngine::ShatterGlassEffect*>(Locator::GetOffScreenRenderer()->GetRenderPass(MiiEngine::OffScreenType::ShatterGlass))) {
			shatterGlass->SetData(data);
		} else {
			assert(false && "ShatterGlassEffectが見つかりませんでした。");
		}
	}
}