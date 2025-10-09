#pragma once
/// ===Include=== ///
// buffer
#include "Engine/Graphics/3d/Base/VertexBuffer3D.h"
#include "Engine/Graphics/3d/Base/IndexBuffer3D.h"
#include "Engine/Graphics/3d/Base/ObjectCommon.h"
// Pipeline
#include "Engine/DataInfo/PipelineStateObjectType.h"
// c++
#include <memory>

/// ===前方宣言=== ///
class GameCamera;

///=====================================================/// 
/// SkyBox
///=====================================================///
class SkyBox {
public:

	SkyBox() = default;
	~SkyBox();
	
	// 初期化　
	void Initialize(const std::string& fileName, LightType type);
	// 更新
	void Update();
	// 描画
	void Draw(BlendMode mode = BlendMode::kBlendModeNone);

public: /// ===Getter=== ///
	// モデル座標
	const Vector3& GetTranslate() const;
	// モデル回転
	const Quaternion& GetRotate() const;
	// モデル拡縮
	const Vector3& GetScale() const;
	// モデルカラー
	const Vector4& GetColor() const;

public: /// ===Setter=== ///
	// モデルTransform
	void SetTranslate(const Vector3& position);
	void SetRotate(const Quaternion& rotate);
	void SetScale(const Vector3& scale);
	// モデルカラー
	void SetColor(const Vector4& color);
	// Light
	void SetLight(LightType type);
	// LightData
	void SetLightData(LightInfo light);
	// 環境マップ
	void SetEnviromentMapData(bool flag, float string);

private:
	/// ===バッファリソース=== ///
	std::unique_ptr<VertexBuffer3D> vertex_;
	std::unique_ptr<IndexBuffer3D> index_;
	std::unique_ptr<ObjectCommon> common_;

	/// ===バッファリソース内のデータを指すポインタ=== ///
	VertexData3D* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;

	/// ===バッファビュー=== ///
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	/// ===定数=== ///
	static constexpr int kVertexCount = 24;
	static constexpr int kIndexCount = 36;

	/// ===UV=== ///
	EulerTransform uvTransform_;

	/// ===カメラ=== ///
	GameCamera* camera_ = nullptr;

	/// ===プリミティブ情報=== ///
	QuaternionTransform worldTransform_;
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	/// ===ファイルパス=== ///
	std::string textureFilePath_;

	/// ===Light=== ///
	LightInfo light_ = {
		40.0f,
		{{ 1.0f, 1.0f, 1.0f, 1.0f } , { 0.0f, -1.0f, 0.0f } ,1.0f},
		{{ 1.0f, 1.0f, 1.0f, 1.0f } , { 0.0f, 0.0f, 0.0f } , 1.0f, 0.0f, 0.0f},
		{{ 1.0f, 1.0f, 1.0f, 1.0f } , { 0.0f, 0.0f, 0.0f } , 0.0f, { 0.0f, 0.0f, 0.0f } , 0.0f, 0.0f, 0.0f}
	};

	/// ===環境マップ=== ///
	EnviromentMapInfo enviromentMapInfo_;

private: /// ===Functions(関数)=== ///

	// VertexDataの書き込み
	void VertexDataWrite();
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

