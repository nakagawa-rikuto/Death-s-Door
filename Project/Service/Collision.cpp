#include "Collision.h"
// Manager
#include "Engine/System/Managers/ColliderManager.h"
// Locator
#include "Locator.h"

namespace Service {
	///-------------------------------------------/// 
	/// 追加
	///-------------------------------------------///
	void Collision::AddCollider(MiiEngine::Collider* collider) { Locator::GetColliderManager()->AddCollider(collider); }

	///-------------------------------------------/// 
	/// 削除
	///-------------------------------------------///
	void Collision::RemoveCollider(MiiEngine::Collider* collider) { Locator::GetColliderManager()->RemoveCollider(collider); }

	///-------------------------------------------/// 
	/// リセット
	///-------------------------------------------///
	void Collision::Reset() { Locator::GetColliderManager()->Reset(); }

	///-------------------------------------------/// 
	/// Setter
	///-------------------------------------------///
	// LightTYpe
	void Collision::SetLight(MiiEngine::LightType lightType) { Locator::GetColliderManager()->SetLight(lightType); }
	// LightData
	void Collision::SetLightData(MiiEngine::LightInfo light) { Locator::GetColliderManager()->SetLightData(light); }
}