#include "ModelManager.h"
// c++
#include <fstream>
// Engine
#include "Engine/System/Managers/TextureManager.h"
// Math
#include "Math/sMath.h"
#include "Math/MatrixMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
ModelManager::~ModelManager() {
	modelDatas_.clear();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void ModelManager::Initialize(TextureManager* texture) {
	assert(texture);
	textureManager_ = texture;
}

///-------------------------------------------/// 
/// ファイルの読み込み
///-------------------------------------------///
void ModelManager::Load(const std::string& baseDirectoryPath, const std::string& Key, const std::string& filename) {
	// 読み込み済みモデルを検索
	if (modelDatas_.contains(filename)) {
		// 読み込み済みなら早期return
		return;
	}

	// Dataの宣言
	ModelData modeldata;
	// モデル読み込み
	modeldata = LoadObjFile(baseDirectoryPath + "/" + Key, filename);

	// テクスチャの読み込みとインデックス設定
	if (!modeldata.material.textureFilePath.empty()) { // 空でなければ
		// TextureManager からテクスチャを読み込み、インデックスを取得
		textureManager_->LoadTexture(modeldata.material.textureFilePath, modeldata.material.textureFilePath);
	}

	// モデルをMapコンテナに格納
	modelDatas_[Key] = modeldata;
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ModelData ModelManager::GetModelData(const std::string& directorPath) {
	assert(modelDatas_.contains(directorPath));
	return modelDatas_.at(directorPath);
}

///-------------------------------------------/// 
/// .mltファイルの読み込み
///-------------------------------------------///
MaterialData ModelManager::LoadMaterialTemplateFile(const std::string& directorPath, const std::string& filename) {
	/// ===必要な変数と宣言ファイルを開く=== ///
	MaterialData materialData; // 構築するMaterialData
	std::string line; // ファイルから読んだ1行を格納するもの
	std::ifstream file(directorPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める
	//materialData.textureFilePath = directorPath + "/" + filename; // テクスチャファイルパスを入れる

	/// ===ファイルを読み、MateiralDataを構築=== ///
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;

			// 連結してファイルパスにする
			materialData.textureFilePath = directorPath + "/" + textureFilename;
		}
	}

	// MaterialDataを返す
	return materialData;
}

ModelData ModelManager::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData; // 構築するModelData

	/// ===assimpでobjを読む=== ///
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); // メッシュがないのは対応しない。
	/// ===meshを解析=== ///
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals()); // 法線がないMeshは今回非対応
		assert(mesh->HasTextureCoords(0)); // TexcoordがないMeshは今回非対応
		modelData.vertices.resize(mesh->mNumVertices); // 最初に頂点数分のメモリを確保しておく
		/// ===頂点を解析する=== ///
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			// 右手系->左手系への変換を忘れずに
			modelData.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };
		}
		/// ===Meshの中身（face）を解析する=== ///
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3); // 三角形のみサポート
			/// ===Faceの中身（index）を解析する=== ///
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}
		// Boneがあるかどうかを確認
		if (mesh->HasBones()) {
			modelData.haveBone = true;
			/// ===Boneの解析=== ///
			for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
				/// ===Jointごとの格納領域を作成=== ///
				aiBone* bone = mesh->mBones[boneIndex];
				std::string jointName = bone->mName.C_Str();
				jointWeightData& jointWeightData = modelData.skinClusterData[jointName];
				/// ===InverseBindPoseMatrixの抽出=== ///
				aiMatrix4x4 bindPoseMatrixAsimp = bone->mOffsetMatrix.Inverse(); // BindePoseMatrxに戻す
				aiVector3D scale, translate;
				aiQuaternion rotate;
				bindPoseMatrixAsimp.Decompose(scale, rotate, translate); // 成分を抽出
				// 左手系のBindPoseMatrixを作成
				Matrix4x4 bindPoseMatrix = Math::MakeAffineQuaternionMatrix(
					{ scale.x, scale.y, scale.z }, { rotate.x, -rotate.y, -rotate.z, rotate.w }, { -translate.x, translate.y, translate.z });
				// InverseBindMatrixにする
				jointWeightData.inverseBindPosematrix = Math::Inverse4x4(bindPoseMatrix);
				/// ===Weight情報を取り出す=== ///
				for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
					jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
				}
			}
		} else {
			modelData.haveBone = false;
		}
		/// ===materialを解析する=== ///
		for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
			aiMaterial* material = scene->mMaterials[materialIndex];
			if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
				aiString textureFilePath;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
				modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
			}
		}
	}
	/// ===assimpでNodeを解析する=== ///
	modelData.rootNode = ReadNode(scene->mRootNode);
	// ModelDataを返す
	return modelData;
}

///-------------------------------------------/// 
/// 構造体Nodeに変換するための関数
///-------------------------------------------///
Node ModelManager::ReadNode(aiNode* node) {
	Node result;
	aiMatrix4x4 aiLocalMatrix = node->mTransformation; // nodeのlocalMatrixを取得
	aiVector3D scale;
	aiVector3D	translate;
	aiQuaternion rotate;
	aiLocalMatrix.Transpose(); // 列ベクトル形式を行ベクトル形式に転置
	std::memcpy(result.localMatrix.m, &aiLocalMatrix, sizeof(aiMatrix4x4)); // aiMatrix4x4 のデータを result.localMatrix にコピー (memcpy を使用)
	node->mTransformation.Decompose(scale, rotate, translate); // assimpの行列からSRTを抽出する関数を利用
	result.transform.scale = { scale.x, scale.y, scale.z }; // Scaleはそのまま
	result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w }; //ｘ軸反転、さらに回転方向が逆なので軸を反転させる
	result.transform.translate = { -translate.x, translate.y, translate.z }; // ｘ軸を反転
	result.localMatrix = Math::MakeAffineQuaternionMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);
	result.name = node->mName.C_Str(); // Node名を格納
	result.children.resize(node->mNumChildren); // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}
