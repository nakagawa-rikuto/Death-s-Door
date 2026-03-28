#include "DeltaTime.h"
// DeltaTime
#include "Engine/System/GameTime/GameTime.h"
// Locator
#include "Locator.h"

namespace Service {
	///-------------------------------------------/// 
	/// Setter
	///-------------------------------------------///
	void DeltaTime::SetDeltaTime(const float& deltaTime) { Locator::GetGameTime()->SetGameTime(deltaTime); }

	///-------------------------------------------/// 
	/// Getter
	///-------------------------------------------///
	const float& DeltaTime::GetDeltaTime() { return Locator::GetGameTime()->GetGameTime(); }
}



