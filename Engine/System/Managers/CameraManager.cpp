#include "CameraManager.h"

#include <cassert>

///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
CameraManager::CameraManager() = default;
CameraManager::~CameraManager() {
	activeCamera_.reset();
}

///-------------------------------------------/// 
/// 全てのカメラの更新
///-------------------------------------------///
void CameraManager::UpdateAllCameras() {
	// 更新をする
	for (auto& [name, camera] : cameras_) {
		camera->Update();
	}
}

///-------------------------------------------/// 
/// 追加
///-------------------------------------------///
void CameraManager::Add(const std::string & name, std::shared_ptr<Camera> camera) {
	cameras_[name] = camera;

	if (!activeCamera_) {
		activeCamera_ = camera; // 最初に追加されたカメラをデフォルトでアクティブにする
	}
}

///-------------------------------------------/// 
/// 削除
///-------------------------------------------///
void CameraManager::Remove(const std::string & name) {
	cameras_.erase(name);

	if (activeCamera_ && cameras_.find(name) == cameras_.end()) {
		activeCamera_ = nullptr;
	}
}

///-------------------------------------------/// 
/// Getter・Setter
///-------------------------------------------///
// Getter
std::shared_ptr<Camera> CameraManager::GetActiveCamera() const { return activeCamera_; }
// Setter
void CameraManager::SetActiveCamera(const std::string& name) {
	// 指定されたカメラが存在する場合アクティブにする
	assert(cameras_[name]);
	if (cameras_.find(name) != cameras_.end()) {
		activeCamera_ = cameras_[name];
	}
}
