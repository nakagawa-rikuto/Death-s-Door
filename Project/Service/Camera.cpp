#include "Camera.h"
// CameraManager
#include "Engine/System/Managers/CameraManager.h"
// Locator
#include "Locator.h"

namespace Service {
	///-------------------------------------------/// 
	/// 追加
	///-------------------------------------------///
	void Camera::AddCamera(const std::string& name, MiiEngine::CameraCommon* camera) {
		Locator::GetCameraManager()->AddCamera(name, camera);
	}

	///-------------------------------------------/// 
	/// 削除
	///-------------------------------------------///
	void Camera::Remove(const std::string& name) {
		Locator::GetCameraManager()->RemoveCamera(name);
	}

	///-------------------------------------------///  
	/// カメラのチェック
	///-------------------------------------------///
	void Camera::HasCamera(const std::string& name) {
		Locator::GetCameraManager()->HasCamera(name);
	}

	///-------------------------------------------/// 
	/// アクティブカメラのGetter
	///-------------------------------------------///
	MiiEngine::CameraCommon* Camera::GetActiveCamera() {
		return Locator::GetCameraManager()->GetActiveCamera();
	}

	///-------------------------------------------/// 
	/// アクティブカメラのSetter
	///-------------------------------------------///
	void Camera::SetActiveCamera(const std::string& name) {
		Locator::GetCameraManager()->SetActiveCamera(name);
	}

}
