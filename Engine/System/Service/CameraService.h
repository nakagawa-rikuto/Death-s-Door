#pragma once
/// ===Include=== ///
// c++
#include <string>
#include <memory>

/// ===前方宣言=== ///
class Camera;

///=====================================================/// 
/// CameraService
///=====================================================///
class CameraService {
public:
	static void Add(const std::string& name, std::shared_ptr<Camera> camera);
	static void Remove(const std::string& name);

	static std::shared_ptr<Camera> GetActiveCamera();
	static void SetActiveCamera(const std::string& name);
};

