#pragma once
/// ===Include=== ///
// DirectX
#include <d3d12.h>
// c++
#include <cstdint>
#include <cassert>
// Engien
#include "Engine/Core/ComPtr.h"

/// ===前方宣言=== ///
class DXCommon;

///=====================================================/// 
/// DSVManager
///=====================================================///
class DSVManager {
public:

	DSVManager();
	~DSVManager();

	// 初期化
	void Initialize(DXCommon* dxcommon);
	// 深度バッファの生成(DSV)
	void CreateDepthBuffer(uint32_t index);
	// クリア
	void ClearDepthBuffer(ID3D12GraphicsCommandList* commandList);
	// 確保関数
	uint32_t Allocate();
	bool AssertAllocate();

public: /// ===Getter=== ///
	// Heap
	ID3D12DescriptorHeap* GetDescriptorHeap() const;
	// CPU
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index) const;
	// GPU
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index) const;

public:/// ===定数=== ///

	// DSVの数
	static const uint32_t kMaxDSVCount_ = 16;

private:
	// DXCommon
	DXCommon* dxcommon_ = nullptr;
	// depthStencilResource
	ComPtr<ID3D12Resource> resource_[kMaxDSVCount_];
	// ヒープ
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	// スクリプタサイズ
	uint32_t descriptorSize_ = 0;
	// 次に使用するSRVインデックス
	uint32_t useIndex_ = 0;
};

