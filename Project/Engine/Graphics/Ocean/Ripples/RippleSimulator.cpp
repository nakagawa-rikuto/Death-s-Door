#include "RippleSimulator.h"
#include "Engine/System/Managers/SRVManager.h"
#include "Service/Locator.h"
#include "Service/Render.h"
#include <cassert>
#include <cmath>

namespace MiiEngine {

    ///-------------------------------------------/// 
    /// デストラクタ
    ///-------------------------------------------///
    RippleSimulator::~RippleSimulator() {
        paramsBuffer_.reset();
        injectionBuffer_.reset();
        pingResource_.reset();
        pongResource_.reset();
    }

    ///-------------------------------------------/// 
    /// SRVインデックスの取得
    ///-------------------------------------------///
    uint32_t RippleSimulator::GetCurrentSRVIndex() const {
        return ((pingPong_ ^ 1) == 0) ? srvPingIndex_ : srvPongIndex_;
    }

    ///-------------------------------------------/// 
    /// 初期化
    ///-------------------------------------------///
    void RippleSimulator::Initialize(ID3D12Device* device, uint32_t gridSize, float gridWidth) {
        // SRVの取得
        srvManager_ = Service::Locator::GetSRVManager();

        // グリッドサイズの取得
        gridSize_ = gridSize;
        gridWidth_ = gridWidth;

        /// ===RippleParamsの定数バッファ=== ///
        paramsBuffer_ = std::make_unique<BufferBase>();
        paramsBuffer_->Create(device, sizeof(RippleParams));
        void* m = nullptr;
        paramsBuffer_->GetBuffer()->Map(0, nullptr, &m);
        paramsData_ = static_cast<RippleParams*>(m);

        injectionBuffer_ = std::make_unique<BufferBase>();
        injectionBuffer_->Create(device, sizeof(RippleInjection));
        void* m2 = nullptr;
        injectionBuffer_->GetBuffer()->Map(0, nullptr, &m2);
        injectionData_ = static_cast<RippleInjection*>(m2);

        /// ===UAVインデックス管理=== ///
        uint32_t base = srvManager_->AllocateContiguous(2);
        uavIndices_[0] = base;
        uavIndices_[1] = base + 1;
        // u0:Ping
        CreateTextureUAV(device, pingResource_, pingUAV_, 0, gridSize, gridSize);
        // u1:Pong
        CreateTextureUAV(device, pongResource_, pongUAV_, 1, gridSize, gridSize);

        // SRV(t3)
        uint32_t srvBase = srvManager_->AllocateContiguous(2);
        srvPingIndex_ = srvBase;
        srvPongIndex_ = srvBase + 1;
        // 
        srvManager_->CreateSRVForTexture2D(srvPingIndex_, pingResource_->GetBuffer(), DXGI_FORMAT_R32G32B32A32_FLOAT, 1);
        srvManager_->CreateSRVForTexture2D(srvPongIndex_, pongResource_->GetBuffer(), DXGI_FORMAT_R32G32B32A32_FLOAT, 1);

        // 初期値
        params_.gridSize = gridSize_;
        params_.cellSize = gridWidth_ / static_cast<float>(gridSize_);
        params_.waveSpeed = 5.0f;
        params_.damping = 0.990f;
        params_.pingPong = 0;
    }

    ///-------------------------------------------/// 
    /// 更新処理 
    ///-------------------------------------------///
    void RippleSimulator::Update(float deltaTime) {
        params_.deltaTime = deltaTime;
        params_.pingPong = pingPong_;
        *paramsData_ = params_;
    }

    ///-------------------------------------------/// 
    /// 波紋シミュレーション
    ///-------------------------------------------///
    void RippleSimulator::Simulate(ID3D12GraphicsCommandList* commandList) {

        const uint32_t tg = (gridSize_ + kThreadGroupSize_ - 1) / kThreadGroupSize_;
        auto CBV0 = paramsBuffer_->GetBuffer()->GetGPUVirtualAddress();
        auto UAVTable = srvManager_->GetGPUDescriptorHandle(uavIndices_[0]);

        // どちらのバッファを読み・書きするかはシェーダー内のPingPongで制御
        Service::Render::SetCSPSO(commandList, CSPipelineType::Ripples, L"SimulateRipple");
        commandList->SetComputeRootConstantBufferView(0, CBV0);
        commandList->SetComputeRootDescriptorTable(2, UAVTable);
        commandList->Dispatch(tg, tg, 1);

        // バッファを反転
        UAVBarrier(commandList, pingResource_->GetBuffer());
        UAVBarrier(commandList, pongResource_->GetBuffer());

		// PingPongフラグを反転
        pingPong_ ^= 1;
    }

    ///-------------------------------------------/// 
    /// 波紋の発生
    ///-------------------------------------------///
    void RippleSimulator::AddRipple(ID3D12GraphicsCommandList* commandList, Vector2 uv, float radius, float strength) {

        /// ===パラメーターの書き込み=== ///
        // Injection
        injection_.uv = uv;
        injection_.radius = radius;
        injection_.strength = strength;
        *injectionData_ = injection_;

        // Params
        params_.pingPong = pingPong_; // 現在書き込み対象に注入
        *paramsData_ = params_;

        const uint32_t tg = (gridSize_ + kThreadGroupSize_ - 1) / kThreadGroupSize_;
        auto CBV0 = paramsBuffer_->GetBuffer()->GetGPUVirtualAddress();
        auto CBV1 = injectionBuffer_->GetBuffer()->GetGPUVirtualAddress();
        auto UAVTable = srvManager_->GetGPUDescriptorHandle(uavIndices_[0]);

        Service::Render::SetCSPSO(commandList, CSPipelineType::Ripples, L"AddRipplePoint");
        commandList->SetComputeRootConstantBufferView(0, CBV0);
        commandList->SetComputeRootConstantBufferView(1, CBV1);
        commandList->SetComputeRootDescriptorTable(2, UAVTable);
        commandList->Dispatch(tg, tg, 1);

        UAVBarrier(commandList, pingResource_->GetBuffer());
        UAVBarrier(commandList, pongResource_->GetBuffer());
    }

    ///-------------------------------------------/// 
	/// リソースの状態をSRVに遷移 
    ///-------------------------------------------///
    void RippleSimulator::TransitionToSRV(ID3D12GraphicsCommandList* commandList) {
        // GetCurrentSRVIndex() と対応するリソースを遷移する。
        ID3D12Resource* target = ((pingPong_ ^ 1) == 0) ? pingResource_->GetBuffer() : pongResource_->GetBuffer();

		// SRV状態に遷移
        TransitionBarrier(commandList, target, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    }

    ///-------------------------------------------/// 
	/// リソースの状態をUAVに遷移 
    ///-------------------------------------------///
    void RippleSimulator::TransitionToUAV(ID3D12GraphicsCommandList * commandList) {
		// GetCurrentSRVIndex() と対応するリソースを遷移する。
        ID3D12Resource* target = ((pingPong_ ^ 1) == 0) ? pingResource_->GetBuffer() : pongResource_->GetBuffer();

		// UAV状態に遷移
        TransitionBarrier(commandList, target, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    }

    ///-------------------------------------------/// 
    /// テクスチャUAVリソースの生成
    ///-------------------------------------------///
    void RippleSimulator::CreateTextureUAV(
        ID3D12Device* device,
        std::unique_ptr<BufferBase>& outResource,
        UAV& outUAV,
        int uavSlot,
        uint32_t width, uint32_t height,
        DXGI_FORMAT format) {

        HRESULT hr;

        // Texture2D リソースとして作成
        D3D12_RESOURCE_DESC texDesc{};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.DepthOrArraySize = 1;
        texDesc.MipLevels = 1;
        texDesc.Format = format; // RWTexture2D<float4>
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        D3D12_HEAP_PROPERTIES heapProps{};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

        // Texture2Dリソースを直接作成
        ComPtr<ID3D12Resource> resource;
        hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            nullptr,
            IID_PPV_ARGS(&resource)
        );
        assert(SUCCEEDED(hr));

        // BufferBaseにリソースを渡す
        outResource = std::make_unique<BufferBase>();
        outResource->SetBuffer(resource.Get()); // Detachして所有権を渡す。

        // UAVの作成
        outUAV.CreateAsTexture2D(
            device,
            outResource->GetBuffer(),
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            srvManager_->GetCPUDescriptorHandle(uavIndices_[uavSlot])
        );
    }

    ///-------------------------------------------/// 
    /// UAVバリア
    ///-------------------------------------------///
    void RippleSimulator::UAVBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource) {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = resource;
        commandList->ResourceBarrier(1, &barrier);
    }

    ///-------------------------------------------/// 
    /// リソース状態の遷移バリア 
    ///-------------------------------------------///
    void RippleSimulator::TransitionBarrier(
        ID3D12GraphicsCommandList* commandList, 
        ID3D12Resource* resource, 
        D3D12_RESOURCE_STATES before, 
        D3D12_RESOURCE_STATES after) {

        D3D12_RESOURCE_BARRIER b = {};
        b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        b.Transition.pResource = resource;
        b.Transition.StateBefore = before;
        b.Transition.StateAfter = after;
        b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &b);
    }
}