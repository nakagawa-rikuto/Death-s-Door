#include "CameraManager.h"

#include <cassert>

///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
CameraManager::CameraManager() = default;
CameraManager::~CameraManager() {
	activeCamera_.reset();
	cameras_.clear();
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
/// カメラを追加
///-------------------------------------------///
void CameraManager::AddCamera(const std::string& name, std::shared_ptr<GameCamera> camera) {
	cameras_[name] = camera;

	if (!activeCamera_) {
		activeCamera_ = camera; // 最初に追加されたカメラをデフォルトでアクティブにする
	}
}

///-------------------------------------------/// 
/// カメラを削除
///-------------------------------------------///
void CameraManager::RemoveCamera(const std::string& name) {
	auto it = cameras_.find(name);
	if (it != cameras_.end()) {
		// アクティブカメラが削除されるカメラの場合、activeCamera_をnullptrにする
		if (activeCamera_ == it->second) {
			activeCamera_ = nullptr;
		}
		cameras_.erase(it);
	}
}

///-------------------------------------------/// 
/// カメラが存在するかチェック
///-------------------------------------------///
bool CameraManager::HasCamera(const std::string& name) const {
	return cameras_.find(name) != cameras_.end();
}

///-------------------------------------------/// 
/// Getter・Setter
///-------------------------------------------///
// 指定されたカメラのGetter
std::shared_ptr<GameCamera> CameraManager::GetCamera(const std::string& name) const {
	auto it = cameras_.find(name);
	if (it != cameras_.end()) {
		return it->second;
	}
	return nullptr;
}

// アクティブカメラのGetter
std::shared_ptr<GameCamera> CameraManager::GetActiveCamera() const {
	return activeCamera_;
}


// アクティブカメラのSetter
void CameraManager::SetActiveCamera(const std::string& name) {
	auto it = cameras_.find(name);
	if (it != cameras_.end()) {
		activeCamera_ = it->second;
	} else {
		// カメラが存在しない場合の警告
		assert(false && "指定されたカメラが存在しません");
	}
}