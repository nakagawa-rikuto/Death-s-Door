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
/// RTVManager
///=====================================================///
class RTVManager {
public:

	RTVManager();
	~RTVManager();

	// 初期化
	void Initialize(DXCommon* dxcommon);
	// 作成
	void CreateRenderTarget(uint32_t index, ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc);
	// クリア
	void ClearRenderTarget(ID3D12GraphicsCommandList* commandList, uint32_t index, const float color[4]);
	// 確保関数
	uint32_t Allocate();
	bool AssertAllocate();

	// SwapChaiのRTVの生成
	void CreateSwapChainRenderTargets();

public: /// ===Getter=== ///
	// Heap
	ID3D12DescriptorHeap* GetDescriptorHeap() const;
	// CPU
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index) const;
	// GPU
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index) const;

public:/// ===定数=== ///

	// RTVの数
	static const uint32_t kMaxRTVCount_ = 16;

private:
	// DXCommon
	DXCommon* dxcommon_ = nullptr;
	// ヒープ
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	// デスクリプタハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandles_[kMaxRTVCount_];
	// スクリプタサイズ
	uint32_t descriptorSize_ = 0;
	// 次に使用するSRVインデックス
	uint32_t useIndex_ = 0;
};

