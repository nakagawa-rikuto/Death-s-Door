#pragma once
/// ===include=== ///
// Engine
#include "Engine/Core/ComPtr.h"
#include "Engine/DataInfo/CData.h"
// DirectXTex
#include "DirectXTex.h"
// C++
#include <string>
#include <map>
// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/// ===前方宣言=== ///
class TextureManager;

///=====================================================///  
/// モデルマネージャ
///=====================================================///
class ModelManager {
public:/// ===基本的な関数=== ///

	ModelManager() = default;
	~ModelManager();

public:/// ===Functions(関数)=== ///

	// 初期化
	void Initialize(TextureManager* texture);

	// ファイルの読み込み
	void Load(const std::string& Key, const std::string& baseDirectoryPath, const std::string& filename);

	// モデルデータの取得
	ModelData GetModelData(const std::string& filename);

private:/// ===Variables(変数)=== ///

	// テクスチャマネージャ
	TextureManager* textureManager_ = nullptr;

	// モデルデータ
	std::map<std::string, ModelData> modelDatas_;

private:/// ===Functions(関数)=== ///
	// .mtlファイルの読み込み
	MaterialData LoadMaterialTemplateFile(const std::string& directorPath, const std::string& filename);

	// .objファイルの読み込み
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	// assimpのNode(ainode* node)から、構造体Nodeに変換する関数
	Node ReadNode(aiNode* node);
};

