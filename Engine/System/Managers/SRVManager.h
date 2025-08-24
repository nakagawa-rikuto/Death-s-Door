#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Core/ComPtr.h"
// DirectX
#include <d3d12.h>
// c++
#include <cstdint>
#include <cassert>

/// ===前方宣言=== ///
class DXCommon;

///=====================================================/// 
/// SRVManager
///=====================================================///
class SRVManager {
public:

	SRVManager();
	~SRVManager();

	// 初期化
	void Initialize(DXCommon* dxcommon);
	// 描画前処理
	void PreDraw();
	// SRVの作成
	void CreateSRV(uint32_t srvIndex, ID3D12Resource* pResource, D3D12_SHADER_RESOURCE_VIEW_DESC desc);
	// 確保関数
	uint32_t Allocate();
	bool AssertAllocate();

	/// <summary>
	/// SRV生成（テクスチャ用）
	/// </summary>
	void CreateSRVForTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

	// SRVの生成(cubemap用)
	void CreateSRVForCubeMap(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

	/// <summary>
	/// SRV生成（Structured Buffer用）
	/// </summary>
	void CreateSRVForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

public:/// ===Setter=== ///

	/// <summary>
	///	SRVセットコマンド
	/// </summary>
	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

public:/// ===Getter=== ///

	ID3D12DescriptorHeap* GetDescriptorHeap() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

public:/// ===定数=== ///

	// 最大SRV数（最大テクスチャ数）
	static const uint32_t kMaxSRVCount_;

private:/// ===variable=== ///
	DXCommon* dXCommon_ = nullptr; // DXCommon
	
	// SRV用のでスクリプタサイズ
	uint32_t descriptorSize_;

	// SRV用でスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	// 次に使用するSRVインデックス
	uint32_t useIndex_ = 0;
};


