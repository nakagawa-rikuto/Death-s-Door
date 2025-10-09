#pragma once
/// ===Include=== ///
// GameCamera
#include "application/Game/Camera/GameCamera.h"
// C++
#include <unordered_map>
#include <string>
#include <memory>

///=====================================================/// 
/// カメラマネージャ
///=====================================================///
class CameraManager {
public:/// ===基本的な関数=== ///

	CameraManager();
	~CameraManager();

	/// <summary>
	/// 全てのカメラの更新
	/// </summary>
	void UpdateAllCameras();

public:/// ===関数=== ///

	/// <summary>
	/// カメラを追加
	/// </summary>
	void AddCamera(const std::string& name, std::shared_ptr<GameCamera> camera);

	/// <summary>
	/// カメラを削除
	/// </summary>
	void RemoveCamera(const std::string& name);

	/// <summary>
	/// カメラが存在するかチェック
	/// </summary>
	bool HasCamera(const std::string& name) const;

public:/// ===Setter・Getter=== ///

	// 指定されたカメラのGetter
	std::shared_ptr<GameCamera> GetCamera(const std::string& name) const;

	// アクティブカメラのGetter
	std::shared_ptr<GameCamera> GetActiveCamera() const;
	// アクティブカメラのSetter
	void SetActiveCamera(const std::string& name);

private:
	// カメラを管理するマップ
	std::unordered_map<std::string, std::shared_ptr<GameCamera>> cameras_;

	// 現在アクティブなカメラ
	std::shared_ptr<GameCamera> activeCamera_;
};