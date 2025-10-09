#pragma once
/// ===Include=== ///
// c++
#include <string>
#include <memory>

/// ===前方宣言=== ///
class GameCamera;

///=====================================================/// 
/// CameraService
///=====================================================///
class CameraService {
public:

	static void AddCamera(const std::string& name, std::shared_ptr<GameCamera> camera);
	static void Remove(const std::string& name);
	static void HasCamera(const std::string& name);

	static std::shared_ptr<GameCamera> GetActiveCamera();
	static void SetActiveCamera(const std::string& name);
};

