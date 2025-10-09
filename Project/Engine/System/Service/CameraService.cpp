#include "CameraService.h"
// CameraManager
#include "Engine/System/Managers/CameraManager.h"
// ServiceLovator
#include "ServiceLocator.h"

// 追加
void CameraService::AddCamera(const std::string& name, std::shared_ptr<GameCamera> camera) {
	ServiceLocator::GetCameraManager()->AddCamera(name, camera);
}
// 削除
void CameraService::Remove(const std::string& name) {
	ServiceLocator::GetCameraManager()->RemoveCamera(name);
}
// カメラのチェック
void CameraService::HasCamera(const std::string& name) {
	ServiceLocator::GetCameraManager()->HasCamera(name);
}
// アクティブカメラのGetter
std::shared_ptr<GameCamera> CameraService::GetActiveCamera() {
	return ServiceLocator::GetCameraManager()->GetActiveCamera();
}
// アクティブカメラのSetter
void CameraService::SetActiveCamera(const std::string& name) {
	ServiceLocator::GetCameraManager()->SetActiveCamera(name);
}
