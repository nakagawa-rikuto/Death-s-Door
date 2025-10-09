#include "DSVManager.h"
// Engine
#include "Engine/Core/DXCommon.h"
// Service
#include "Engine/System/Service/GraphicsResourceGetter.h"

///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
DSVManager::DSVManager() = default;
DSVManager::~DSVManager() {
    descriptorHeap_.Reset();
    resource_->Reset();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// Heap
ID3D12DescriptorHeap* DSVManager::GetDescriptorHeap() const { return descriptorHeap_.Get(); }
// CPU
D3D12_CPU_DESCRIPTOR_HANDLE DSVManager::GetCPUDescriptorHandle(uint32_t index) const {
    D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSize_;
    return handle;
}
// GPU
D3D12_GPU_DESCRIPTOR_HANDLE DSVManager::GetGPUDescriptorHandle(uint32_t index) const {
    D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSize_;
    return handle;
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void DSVManager::Initialize(DXCommon* dxcommon) {
    dxcommon_ = dxcommon;
    // DescriptorSizeの取得, Heapの生成
    descriptorHeap_ = dxcommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxDSVCount_, false);
    descriptorSize_ = dxcommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

///-------------------------------------------/// 
/// クリア
///-------------------------------------------///
void DSVManager::ClearDepthBuffer(ID3D12GraphicsCommandList* commandList) {
    for (uint32_t i = 0; i < useIndex_; ++i) {
        commandList->ClearDepthStencilView(GetCPUDescriptorHandle(i), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }
}

///-------------------------------------------/// 
/// 確保関数
///-------------------------------------------///
uint32_t DSVManager::Allocate() {
    /// ===上限に達していないかチェックしてassert=== ///
    assert(AssertAllocate());
    // return する番号をいったん記録しておく
    int index = useIndex_;
    // 次回のために番号を1進める
    useIndex_++;
    // 上で記録した番号をreturn(0番はImGuiだから+1)
    return index;
}
bool DSVManager::AssertAllocate() { return useIndex_ < kMaxDSVCount_; }

///-------------------------------------------/// 
/// 生成
///-------------------------------------------///
void DSVManager::CreateDepthBuffer(uint32_t index) {
    D3D12_RESOURCE_DESC desc{};
    desc.Width = GraphicsResourceGetter::GetWindowWidth();
    desc.Height = GraphicsResourceGetter::GetWindowHeight();
    desc.MipLevels = 1;
    desc.DepthOrArraySize = 1;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count = 1;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    clearValue.DepthStencil.Depth = 1.0f;

    dxcommon_->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&resource_[index]));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = desc.Format;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    dxcommon_->GetDevice()->CreateDepthStencilView(
        resource_[index].Get(), &dsvDesc, GetCPUDescriptorHandle(index));

    // この部分があっているのかが良くわからない。
    useIndex_ = index + 1;
}
