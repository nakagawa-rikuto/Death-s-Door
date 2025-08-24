#pragma once
/// ===include=== ///
// Buffer
#include "Engine/Graphics/3d/Base/VertexBuffer3D.h"
#include "Engine/Graphics/3d/Base/IndexBuffer3D.h"
#include "Engine/Graphics/3d/Base/ObjectCommon.h"
// Data
#include "Engine/DataInfo/BlendModeData.h"
// c++
#include <memory>
#include <string>

/// ===前方宣言=== ///
class Camera;

///=====================================================/// 
/// プリミティブコモン
///=====================================================///
class Primitive3DCommon {
public:
	Primitive3DCommon();
	~Primitive3DCommon();

	// 初期化
	virtual void Initialize(const std::string& modelName, LightType type) = 0;
	// 更新
	virtual void Update();
	// 描画
	virtual void Draw(BlendMode mode) = 0;

	// 作成
	void Create(ID3D12Device* device, LightType type);
	// 描画処理
	void Bind(ID3D12GraphicsCommandList* commandList);

public:/// ===Setter=== ///
	// Light
	void SetLightType(LightType type);

protected: /// ===継承先で使用する変数=== ///

	// バッファリソース
	std::shared_ptr<VertexBuffer3D> vertex_;
	std::shared_ptr<IndexBuffer3D> index_;

	/// ===バッファビュー=== ///
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	/// ===バッファリソース内のデータを指すポインタ=== ///
	VertexData3D* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;

	/// ===Primitiv情報=== ///
	EulerTransform uvTransform_;
	QuaternionTransform worldTransform_;
	Vector4 color_;

	/// ===ファイルパス=== ///
	std::string textureFilePath_;

	/// ===Light=== ///
	LightInfo light_;

	/// ===Camera=== ///
	Camera* camera_ = nullptr;

	/// ===環境マップ=== ///
	EnviromentMapInfo enviromentMapInfo_;

private:/// ===Variables(変数)=== ///

	// Common
	std::shared_ptr<ObjectCommon> common_;

private:
	// MaterialDataの書き込み
	void MateialDataWrite();
	// Transform情報の書き込み
	void TransformDataWrite();
	// LightData書き込み
	void LightDataWrite();
	// CameraData書き込み
	void CameraDataWrite();
	// EnvironmentMapData書き込み
	void EnviromentMapDataWrite();
};

