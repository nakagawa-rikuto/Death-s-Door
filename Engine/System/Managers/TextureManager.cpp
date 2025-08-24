#include "TextureManager.h"
// Engine
#include "Engine/Core/DXCommon.h"
#include "Engine/Core/StringUtility.h"
#include "Engine/System/Managers/SRVManager.h"
// Function
#include "Engine/DataInfo/FunctionData.h"
// c++
#include <cassert>

// StringUtility
using namespace StringUtility;

// ImGuiで0番を使用するため、１番から使用
uint32_t TextureManager::kSRVIndexTop_ = 1;

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
TextureManager::~TextureManager() {
	textureDatas_.clear();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// メタデータの取得
const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& Key) {
	TextureData& textureData = textureDatas_[Key];
	return textureData.metadata;
}
// GPUハンドルの取得
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSRVHandleGPU(const std::string& Key) {
	TextureData& textureData = textureDatas_[Key];
	return textureData.srvHandleGPU;
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// テクスチャの設定
void TextureManager::SetGraphicsRootDescriptorTable(
	ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex, std::string Key) {
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, textureDatas_[Key].srvHandleGPU);
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void TextureManager::Initialize(DXCommon* dxCommon, SRVManager* srvManager) {

	assert(dxCommon);
	assert(srvManager);

	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	// SRVの数と同数
	textureDatas_.reserve(SRVManager::kMaxSRVCount_);
}

///-------------------------------------------/// 
/// テクスチャファイルの読み込み	
///-------------------------------------------///
void TextureManager::LoadTexture(const std::string& key, const std::string& filePath) {

	// 読み込み済みのテクスチャを検索
	if (textureDatas_.contains(key)) {
		assert(srvManager_->AssertAllocate());
		return;
	}

	// テクスチャの上限チェック
	assert(srvManager_->Allocate());

	// テクスチャデータを追加して書き込む
	TextureData& textureData = textureDatas_[key];
	// テクスチャデータの読み込み
	textureData.filePath = filePath;
	DirectX::ScratchImage mipImages = Load(key, filePath); // ミップマップの作成
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = CreateTextureResource(textureData.metadata);
	// テクスチャを転送
	textureData.intermediateResource = UploadTextureData(textureData.resource,  mipImages);

	// SRVを作成するDescriptorHeapの場所設定
	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	// SRVの生成
	if (textureData.metadata.IsCubemap()) {
		// CubMap
		srvManager_->CreateSRVForCubeMap(
			textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format, UINT_MAX);
	} else {
		// Texture2D
		srvManager_->CreateSRVForTexture2D(
			textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format, UINT(textureData.metadata.mipLevels));
	}
	
}


///-------------------------------------------/// 
/// ミップマップの作成
///-------------------------------------------///
DirectX::ScratchImage TextureManager::Load(const std::string& key, const std::string& filePath) {
	
	// テクスチャファイルを読み込んでプログラムで扱えるよにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr;

	// 拡張子の判別
	if (filePath.ends_with(".dds") || filePath.ends_with(".DDS")) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	} else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	 
	// リリースでもエラーが出るようにする
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to load texture with key: " + key + ", from file: " + filePath);
	}

	/// ===ミップマップの対応処理=== ///
	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	if (DirectX::IsCompressed(image.GetMetadata().format)) { // 圧縮フォーマットかどうかを調べる
		mipImages = std::move(image); // 圧縮フォーマットならそのまま使うのでmoveする
	} else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
	}
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to generate mipmaps for texture with key: " + key + ", from file: " + filePath);
	}

	// ミップマップのデータを返す
	return mipImages;
}

///-------------------------------------------/// 
/// TextureResourceの作成
///-------------------------------------------///
ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata) {
	/// ***************************
	/// metadataを基にResourceの設定
	/// ***************************
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); // Textureの幅
	resourceDesc.Height = UINT(metadata.height); // Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行 or 配列Textureの配列数
	resourceDesc.Format = metadata.format; // TextureのFormat
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使っているのは２次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE; // SRV作成可能に 

	/// ***************************
	/// 利用するHeapの設定
	/// ***************************
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // デフォルトHeapに設定
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; // プロセッサの近くに配置
	// マルチGPUを使用していなければ1で大丈夫
	heapProperties.CreationNodeMask = 1;  // ノード0で作成
	heapProperties.VisibleNodeMask = 1;   // ノード0から見える

	/// ***************************
	/// Resourceの生成
	/// ***************************
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, // データを転送される設定
		nullptr, // Clear最適値。
		IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

///-------------------------------------------/// 
/// データを転送する
///-------------------------------------------///
ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages) {

	/// ===IntermediateResource(中間リソース)=== ///
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	// PrepareUploadを利用して、読み込んだデータからDirectX12用のサブリソースの配列を作成
	DirectX::PrepareUpload(dxCommon_->GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	// Subresourceの数を元に、コピー元となるIntermediateRewsourceに必要なサイズを計算
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	// 計算したサイズでIntermediateResourceを作成　
	ComPtr<ID3D12Resource> intermediateResource = nullptr;
	intermediateResource = CreateBufferResourceComPtr(dxCommon_->GetDevice(), intermediateSize);

	/// ===データ転送コマンドに積む=== ///
	UpdateSubresources(dxCommon_->GetCommandList(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	/// ===ResourceStateを変更し、IntermediateResourceを返す=== ///
	// Tetrueへの転送後は利用できるようにD3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);

	return intermediateResource;
}