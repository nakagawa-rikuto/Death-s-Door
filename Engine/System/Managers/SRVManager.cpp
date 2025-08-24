#include "SRVManager.h"
// Engine
#include "Engine/Core/DXCommon.h"

const uint32_t SRVManager::kMaxSRVCount_ = 512;

///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
SRVManager::SRVManager() = default;
SRVManager::~SRVManager(){}


///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void SRVManager::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex) {
	dXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}


///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// DescriptorHeap
ID3D12DescriptorHeap* SRVManager::GetDescriptorHeap() const { return descriptorHeap_.Get(); }
// CPUHandl
D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}
// GPUHandl
D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize_ * index);
	return handleGPU;
}


///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void SRVManager::Initialize(DXCommon* dxcommon) {

	// 引数で受け取ってメンバ変数に記録する
	dXCommon_ = dxcommon;

	// デスクリプタヒープの生成
	descriptorHeap_ = dXCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount_, true);

	// デスクリプタ1個分のサイズを取得して記録
	descriptorSize_ = dXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}


///-------------------------------------------/// 
/// 描画前処理
///-------------------------------------------///
void SRVManager::PreDraw() {

	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
	dXCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

///-------------------------------------------/// 
/// SRVの作成
///-------------------------------------------///
void SRVManager::CreateSRV(uint32_t srvIndex, ID3D12Resource* pResource, D3D12_SHADER_RESOURCE_VIEW_DESC desc) {
	dXCommon_->GetDevice()->CreateShaderResourceView(pResource, &desc, GetCPUDescriptorHandle(srvIndex));
}


///-------------------------------------------/// 
/// 確保関数
///-------------------------------------------///
uint32_t SRVManager::Allocate() {
	/// ===上限に達していないかチェックしてassert=== ///
	assert(AssertAllocate());
	// return する番号をいったん記録しておく
	int index = useIndex_;
	// 次回のために番号を1進める
	useIndex_++;
	// 上で記録した番号をreturn(0番はImGuiだから+1)
	return index + 1;
}
// 上限チャック
bool SRVManager::AssertAllocate() { return useIndex_ < kMaxSRVCount_; }


///-------------------------------------------/// 
/// SRV生成（）
///-------------------------------------------///
// テクスチャ用
void SRVManager::CreateSRVForTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels) {

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = MipLevels;

	// SRVを作成
	CreateSRV(srvIndex, pResource, srvDesc);
}
// CubMap用
void SRVManager::CreateSRVForCubeMap(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels) {

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0; // unionがTextureCubeになったが、内部パラメータの意味はTexture2Dと変わらない
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	// SRVを作成
	CreateSRV(srvIndex, pResource, srvDesc);
}
// Struct Buffer用
void SRVManager::CreateSRVForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride) {

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;  // バッファビュー
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;  // フォーマットはなし
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.StructureByteStride = structureByteStride;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	// SRVを作成
	CreateSRV(srvIndex, pResource, srvDesc);
}