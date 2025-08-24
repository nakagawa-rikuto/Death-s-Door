#include "ParticleCommon.h"
// c++
#include <cassert>
// Math
#include "Math/MatrixMath.h"


///-------------------------------------------/// 
/// コンストラクタ・デストラクタ
///-------------------------------------------///
ParticleCommon::ParticleCommon() = default;
ParticleCommon::~ParticleCommon() { 
	material_.reset(); 
	instancing_.Reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void ParticleCommon::Initlize(ID3D12Device* device, const uint32_t kNumMaxInstance) {

	/// ===mateial=== ///
	// 生成
	material_ = std::make_unique<Material3D>();
	// buffer
	material_->Create(device, sizeof(MaterialData3D));
	material_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = Math::MakeIdentity4x4();

	/// ===Instancing=== ///
	// 生成
	Create(device, sizeof(ParticleForGPU) * kNumMaxInstance);
	// 書き込むためのアドレスを取得
	instancing_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	
	// Dataの書き込み
	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
		instancingData_[index].WVP = Math::MakeIdentity4x4();
		instancingData_[index].World = Math::MakeIdentity4x4();
		instancingData_[index].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}
}

///-------------------------------------------/// 
/// 描画準備
///-------------------------------------------///
void ParticleCommon::Bind(ID3D12GraphicsCommandList* commandList) {
	// マテリアルCBufferの場所設定
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetBuffer()->GetGPUVirtualAddress());
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// Instancing
ID3D12Resource* ParticleCommon::GetInstancing() { return instancing_.Get(); }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// material
void ParticleCommon::SetMatiarlData(const Vector4& color, const Matrix4x4& uvTransform) {
	materialData_->color = color;
	materialData_->uvTransform = uvTransform;

}
// Instancing
void ParticleCommon::SetInstancingData(size_t index, const Vector4& color, const Matrix4x4& WVP, const Matrix4x4& World) {
	instancingData_[index].color = color;
	instancingData_[index].WVP = WVP;
	instancingData_[index].World = World;
}

///-------------------------------------------/// 
/// Bufferの生成
///-------------------------------------------///
void ParticleCommon::Create(ID3D12Device* device, size_t sizeInBytes) {
	HRESULT hr;

	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;  // UploadHeapを使う

	// リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};

	// バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes; // リソースのサイズ。今回はVector4を3頂点分

	// バッファの場合はこれらは1にする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;

	// バッファの場合はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 実際に頂点リソースを作る
	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&instancing_));
	assert(SUCCEEDED(hr));
}
