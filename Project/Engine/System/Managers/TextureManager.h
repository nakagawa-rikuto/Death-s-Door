#pragma once
/// ===include=== ///
// Engine
#include "Engine/Core/ComPtr.h"
#include "Engine/DataInfo/CData.h"
// DirectXTex
#include "DirectXTex.h"
// C++
#include <string>
#include <unordered_map>
// DirectX
#include <d3dx12.h>

/// ===前方宣言=== ///
class DXCommon;
class SRVManager;

///=====================================================/// 
/// テクスチャマネージャ
///=====================================================///
class TextureManager {

public:/// ===基本的な関数=== ///
	
	TextureManager() = default;
	~TextureManager();

	// 初期化
	void Initialize(DXCommon* dxCommon, SRVManager* srvManager);

public:/// ===Getter=== ///

	// テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandleGPU(const std::string& Key);
	// メタデータ
	const DirectX::TexMetadata& GetMetaData(const std::string& Key);

public:/// ==Setter==== ///

	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex, std::string Key);

public:/// ===Variables(変数)=== ///

	/// ===ImGui対応=== ///
	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop_;

public:/// ===Functions(関数)=== ///

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath"></param>
	void LoadTexture(const std::string& key, const std::string& filePath);

private:/// ===Variables(変数)=== ///

	DXCommon* dxCommon_ = nullptr;
	SRVManager* srvManager_ = nullptr;

	/// ===テクスチャデータの構造体=== ///
	// テクスチャ1枚分のデータ
	struct TextureData {
		std::string filePath;
		DirectX::TexMetadata metadata;
		ComPtr<ID3D12Resource> resource;
		ComPtr<ID3D12Resource> intermediateResource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	/// ===テクスチャデータコンテナ=== ///
	// テクスチャデータ
	std::unordered_map<std::string, TextureData> textureDatas_;

private:/// ===Functions(関数)=== ///
	// ミップマップの生成
	DirectX::ScratchImage Load(const std::string& key, const std::string& filePath);
	// TextureResourceの作成
	ComPtr<ID3D12Resource> CreateTextureResource( const DirectX::TexMetadata& metadata);
	// データを転送する
	[[nodiscard]]
	ComPtr<ID3D12Resource> UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);
};

