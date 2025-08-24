#pragma once
/// ===Include=== ///
// Engine
#include "application/Game/Camera/Camera.h"
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
	/// 追加
	/// </summary>
	void Add(const std::string& name, std::shared_ptr<Camera> camera);

	/// <summary>
	/// 削除
	/// </summary>
	void Remove(const std::string& name);

public:/// ===Setter・Getter=== ///

	// アクティブカメラのGetter
	std::shared_ptr<Camera> GetActiveCamera()const;
	// アクティブカメラのSetter
	void SetActiveCamera(const std::string& name);

private:
	// カメラを管理するマップ
	std::unordered_map<std::string, std::shared_ptr<Camera>> cameras_;

	// 現在アクティブなカメラ
	std::shared_ptr<Camera> activeCamera_;
};

