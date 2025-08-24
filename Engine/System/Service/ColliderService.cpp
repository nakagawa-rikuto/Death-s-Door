#include "ColliderService.h"
// Manager
#include "Engine/System/Managers/ColliderManager.h"
// Service
#include "ServiceLocator.h"

///-------------------------------------------/// 
/// 追加
///-------------------------------------------///
void ColliderService::AddCollider(Collider* collider) { ServiceLocator::GetColliderManager()->AddCollider(collider); }

///-------------------------------------------/// 
/// 削除
///-------------------------------------------///
void ColliderService::RemoveCollider(Collider* collider) { ServiceLocator::GetColliderManager()->RemoveCollider(collider); }

///-------------------------------------------/// 
/// リセット
///-------------------------------------------///
void ColliderService::Reset() { ServiceLocator::GetColliderManager()->Reset(); }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// LightTYpe
void ColliderService::SetLight(LightType lightType) {ServiceLocator::GetColliderManager()->SetLight(lightType);}

// LightData
void ColliderService::SetLightData(LightInfo light) {ServiceLocator::GetColliderManager()->SetLightData(light);}