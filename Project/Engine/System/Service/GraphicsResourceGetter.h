#pragma once
/// ===Include=== ///
// C++
#include <string>
#include <d3d12.h>
// DirectXTex
#include "DirectXTex.h"
// Engine
#include "Engine/DataInfo/CData.h"
#include "Engine/DataInfo/AnimationData.h"
#include "Engine/DataInfo/LevelData.h"

///=====================================================/// 
/// GraphicsResourceGetter
///=====================================================///
class GraphicsResourceGetter {
public: /// ===DescriptorHandle=== ///
	// RTVの指定番号のCPUでスクリプタハンドルを取得する
	static D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle(uint32_t index);
	// RTVの指定番号のGPUでスクリプタハンドルを取得する
	static D3D12_GPU_DESCRIPTOR_HANDLE GetRTVGPUDescriptorHandle(uint32_t index);
	// DSVの指定番号のCPUでスクリプタハンドルを取得する
	static D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);
	// DSVの指定番号のGPUでスクリプタハンドルを取得する
	static D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);
	// SRVの指定番号のCPUでスクリプタハンドルを取得する
	static D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	// SRVの指定番号のGPUでスクリプタハンドルを取得する
	static D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

public:/// ===Window=== ///
	// ウィンドウの幅を取得する
	static const int GetWindowWidth();
	// ウィンドウの高さを取得する
	static const int GetWindowHeight();

public:/// ===Data=== ///
	// TextureDataの取得
	static const DirectX::TexMetadata& GetMetaData(const std::string& key);
	// ModelDataの取得
	static ModelData GetModelData(const std::string& directorPath);
	// AnimationDataの取得
	static std::map<std::string, Animation> GetAnimationData(const std::string& directorPath);
	// LevelDataの取得
	static LevelData* GetLevelData(const std::string& file_name);

public:/// ===ID3D12=== ///
	// Deviceの取得
	static ID3D12Device* GetDXDevice();
	static ID3D12GraphicsCommandList* GetDXCommandList();
};