#include "RTVManager.h"
// Engine
#include "Engine/Core/DXCommon.h"

///-------------------------------------------/// 
/// コンストラクタ・デストラクタ
///-------------------------------------------///
RTVManager::RTVManager() = default;
RTVManager::~RTVManager() {
	descriptorHeap_.Reset();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// Heap
ID3D12DescriptorHeap* RTVManager::GetDescriptorHeap() const { return descriptorHeap_.Get(); }
// CPU
D3D12_CPU_DESCRIPTOR_HANDLE RTVManager::GetCPUDescriptorHandle(uint32_t index) const {
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += index * descriptorSize_;
	return handle;
}
// GPU
D3D12_GPU_DESCRIPTOR_HANDLE RTVManager::GetGPUDescriptorHandle(uint32_t index) const {
	D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += index * descriptorSize_;
	return handle;
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void RTVManager::Initialize(DXCommon* dxcommon) {
	dxcommon_ = dxcommon;
	// RTVの生成(レンダーターゲットビュー)
	descriptorHeap_ = dxcommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxRTVCount_, false); // この関数は変更
	descriptorSize_ = dxcommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

///-------------------------------------------/// 
/// 作成
///-------------------------------------------///
void RTVManager::CreateRenderTarget(uint32_t index, ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc) {
	assert(index < kMaxRTVCount_);
	// ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE handle = GetCPUDescriptorHandle(index);

	// RTVの生成
	dxcommon_->GetDevice()->CreateRenderTargetView(resource, &desc, GetCPUDescriptorHandle(index));

	// ハンドルを記録（ClearRenderTargetViewで使うため）
	descriptorHandles_[index] = handle;
}

///-------------------------------------------/// 
/// クリア
///-------------------------------------------///
void RTVManager::ClearRenderTarget(ID3D12GraphicsCommandList* commandList, uint32_t index, const float color[4]) {
	// RTVをクリア
	commandList->ClearRenderTargetView(descriptorHandles_[index], color, 0, nullptr);
}

///-------------------------------------------/// 
/// 確保関数
///-------------------------------------------///
uint32_t RTVManager::Allocate() {
	/// ===上限に達していないかチェックしてassert=== ///
	assert(AssertAllocate());
	// return する番号をいったん記録しておく
	int index = useIndex_;
	// 次回のために番号を1進める
	useIndex_++;
	// 上で記録した番号をreturn(0番はImGuiだから+1)
	return index;
}
bool RTVManager::AssertAllocate() { return useIndex_ < kMaxRTVCount_; }

///-------------------------------------------/// 
/// 作成
///-------------------------------------------///
void RTVManager::CreateSwapChainRenderTargets() {
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// まず1つ目を作る。1つ目は最初のところに作る。作る場所をコリらで指定してあげる必要がある
	descriptorHandles_[0] = dxcommon_->GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, 0);
	CreateRenderTarget(0, dxcommon_->GetSwapChainResource(0), rtvDesc);

	//2つ目のディスクリプタハンドルを得る
	descriptorHandles_[1].ptr = descriptorHandles_[0].ptr + dxcommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	CreateRenderTarget(1, dxcommon_->GetSwapChainResource(1), rtvDesc);

	useIndex_ = 2;
}



