#pragma once
/// ===Include=== ///
// Engine
#include "Engine/DataInfo/CData.h"
#include "Engine/DataInfo/ParticleData.h"
#include "Engine/Core/ComPtr.h"
#include "Engine/Graphics/3d/base/Material3D.h"
// c++
#include <memory>
#include <list>

/// ===前方宣言=== ///
class SRVManager;

///=====================================================/// 
/// ParticleSetUp
///=====================================================///
class ParticleCommon {
public:

	ParticleCommon();
	~ParticleCommon();

	// 初期化
	void Initlize(ID3D12Device* device, const uint32_t kNumMaxInstance);
	// 描画準備
	void Bind(ID3D12GraphicsCommandList* commandList);

public: /// ===Geter=== ///

	// Instancingの取得
	ID3D12Resource* GetInstancing();

public: /// ===Setter=== ///

	// Material
	void SetMatiarlData(const Vector4& color, const Matrix4x4& uvTransform);
	// Instancing
	void SetInstancingData(size_t index, const Vector4& color, const Matrix4x4& WVP, const Matrix4x4& World);

private:

	/// ===バッファリソース=== ///
	std::unique_ptr<Material3D> material_;
	ComPtr<ID3D12Resource> instancing_;
	
	/// ===バッファリソース内のデータを指すポインタ=== ///
	MaterialData3D* materialData_ = nullptr;
	ParticleForGPU* instancingData_ = nullptr;

private:

	// Bufferの生成
	void Create(ID3D12Device* device, size_t sizeInBytes);
};

