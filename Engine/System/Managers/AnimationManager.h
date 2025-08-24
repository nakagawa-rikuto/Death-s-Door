#pragma once
/// ===Include=== ///
#include "Engine/DataInfo/AnimationData.h"
#include "Engine/Core/ComPtr.h"
// DirectXTex
#include "DirectXTex.h"
// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

///=====================================================/// 
/// アニメーションマネージャ
///=====================================================///
class AnimationManager {
public:

	AnimationManager() = default;
	~AnimationManager();

public:
	// ファイル読み込み
	void Load(const std::string& Key, const std::string& baseDirectoryPath, const std::string& filename);

	// Animationの取得
	std::map<std::string, Animation> GetAnimation(const std::string& filename);

private: /// ===Variables(変数)=== ///

	// アニメーションデータ	
	std::map<std::string, std::map<std::string, Animation>> animationDatas_;

private: /// ===Functions(関数)=== ///
	// ファイル読み込み関数
	std::map<std::string, Animation> LoadAnimation(const std::string& directorPath, const std::string& filename);
};

