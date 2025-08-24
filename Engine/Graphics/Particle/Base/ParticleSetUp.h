#pragma once
/// ===Icnlde=== ///
// Buffer
#include "Engine/Graphics/3d/base/VertexBuffer3D.h"
#include "ParticleCommon.h"
// Pipleine
#include "Engine/DataInfo/PipelineStateObjectType.h"
// c++
#include <memory>

/// ===前方宣言=== ///
class Camera;
class SRVManager;

/// ===選択肢=== ///
enum class shapeType {
	// 円
	kCircle,
	// Cylinder
	kCylinder,
	// 総数(これは最後にしなければいけない)
	kNone,
};

///=====================================================/// 
/// Particle共通描画設定
///=====================================================///
class ParticleSetUp {
public:

	ParticleSetUp();
	~ParticleSetUp();

	// 初期化
	void Initialze(const std::string& filename, const uint32_t kNumMaxInstance, shapeType type);
	// 描画
	void Darw(const uint32_t instance, BlendMode mode);

public: /// ===Setter=== ///

	// テクスチャ
	void SetTexture(const std::string& fileName);
	// Material
	void SetMatiarlData(const Vector4& color, const Matrix4x4& uvTransform);
	// Instancing
	void SetInstancingData(size_t index, const Vector4& color, const Matrix4x4& WVP, const Matrix4x4& World);

private:
	/// ===SRV=== ///
	struct ParticleSRV {
		SRVManager* srvManager_ = nullptr;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};
	ParticleSRV srvData_;

	/// ===バッファリソース=== ///
	std::unique_ptr<VertexBuffer3D> vertex_;
	std::unique_ptr<ParticleCommon> common_;

	/// ===バッファリソース内のデータを指すポインタ=== ///
	VertexData3D* vertexData_ = nullptr;

	/// ===バッファビュー=== ///
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	
	/// ===モデルデータ=== ///
	ModelData modelData_;

	/// ===モデル情報=== ///
	uint32_t kNumMaxInstance_;

private:
	/// ===VertexBufferの設定=== ///
	// 円
	void SetVertexBufferCircle();
	// Cylinder
	void SetVertexBufferCylinder();
};

