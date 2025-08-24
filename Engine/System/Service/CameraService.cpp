#include "CameraService.h"
// CameraManager
#include "Engine/System/Managers/CameraManager.h"
// ServiceLovator
#include "ServiceLocator.h"

// 追加
void CameraService::Add(const std::string& name, std::shared_ptr<Camera> camera) {
		ServiceLocator::GetCameraManager()->Add(name, camera);
}
// 削除
void CameraService::Remove(const std::string& name) {
	ServiceLocator::GetCameraManager()->Remove(name);
}
// アクティブカメラのGetter
std::shared_ptr<Camera> CameraService::GetActiveCamera() {
	return ServiceLocator::GetCameraManager()->GetActiveCamera();
}
// アクティブカメラのSetter
void CameraService::SetActiveCamera(const std::string& name) {
	ServiceLocator::GetCameraManager()->SetActiveCamera(name);
}
