#include "FFTOceanCompute.h"
// SRVManager
#include "Engine/System/Managers/SRVManager.h"
// Service
#include "Service/Locator.h"
#include "Service/Render.h"
#include <cassert>

namespace MiiEngine {
	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	FFTOceanCompute::~FFTOceanCompute() {
		// RippleSimulatorのリセット
		ripple_.reset();
		// Unmapしてからリセット
		if (oceanParamsBuffer_ && oceanParamsBuffer_->GetBuffer()) {
			oceanParamsBuffer_->GetBuffer()->Unmap(0, nullptr);
		}
		if (butterflyParamsBuffer_ && butterflyParamsBuffer_->GetBuffer()) {
			butterflyParamsBuffer_->GetBuffer()->Unmap(0, nullptr);
		}
		oceanParamsBuffer_.reset();
		butterflyParamsBuffer_.reset();
		// UAVリソースのリセット
		h0Resource_.reset();
		hktResource_.reset();
		dxResource_.reset();
		dzResource_.reset();
		pingResource_.reset();
		pongResource_.reset();
		displaceResource_.reset();
		normalFoamResource_.reset();
	}

	///-------------------------------------------/// 
	/// Getter
	///-------------------------------------------///
	// 最終変位マップの取得（SRVとして使用）
	ID3D12Resource* FFTOceanCompute::GetDisplacementMap() const { return displaceResource_->GetBuffer(); }
	// 法線・泡マップの取得（SRVとして使用）
	ID3D12Resource* FFTOceanCompute::GetNormalFoamMap() const { return normalFoamResource_->GetBuffer(); }
	// OceanParamsの取得
	OceanParams& FFTOceanCompute::GetOceanParams() { return params_; }
	// SRVDisplaceIndexの取得
	uint32_t FFTOceanCompute::GetSRVDisplaceIndex() const { return srvDisplaceIndex_; }
	// SRVNormalFoamIndexの取得
	uint32_t FFTOceanCompute::GetSRVNormalFoamIndex() const { return srvNormalFoamIndex_; }
	// RippleSimulator
	RippleSimulator* FFTOceanCompute::GetRippleSimulator() const { return ripple_.get(); }

	///-------------------------------------------/// 
	/// Setter
	///-------------------------------------------///
	// OceanParamsの設定
	void FFTOceanCompute::SetOceanParams(const OceanParams& params) { params_ = params; }
	// グリッドサイズの設定
	void FFTOceanCompute::SetGridSize(uint32_t gridSize) { params_.gridSize = gridSize; }

	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	void FFTOceanCompute::Initialize(ID3D12Device* device, int gridSize) {
		assert(device);

		// SRVManagerを取得
		srvManager_ = Service::Locator::GetSRVManager();

		// グリッドサイズ設定
		params_.gridSize = gridSize;
		const uint32_t N = params_.gridSize;

		/// ===OceanParams 定数バッファ=== ///
		oceanParamsBuffer_ = std::make_unique<BufferBase>();
		oceanParamsBuffer_->Create(device, sizeof(OceanParams)); {
			void* mapped = nullptr;
			oceanParamsBuffer_->GetBuffer()->Map(0, nullptr, &mapped);
			oceanParamsData_ = static_cast<OceanParams*>(mapped);
		}

		/// ===ButterflyParams 定数バッファ=== ///
		butterflyParamsBuffer_ = std::make_unique<BufferBase>();
		butterflyParamsBuffer_->Create(device, sizeof(ButterflyParams)); {
			void* mapped = nullptr;
			butterflyParamsBuffer_->GetBuffer()->Map(0, nullptr, &mapped);
			butterflyParamsData_ = static_cast<ButterflyParams*>(mapped);
		}

		/// ===UAVテクスチャリソースの生成=== ///
		// u0?u7の8個の連続したデスクリプタを確保する
		uint32_t baseUAVIndex = srvManager_->AllocateContiguous(8);
		for (int i = 0; i < 8; ++i) {
			uavIndices_[i] = baseUAVIndex + i;
		}
		
		// u0: H0Texture
		CreateTextureUAV(device, h0Resource_, h0UAV_, 0, N, N);
		// u1: HktTexture
		CreateTextureUAV(device, hktResource_, hktUAV_, 1, N, N);
		// u2: DxTexture
		CreateTextureUAV(device, dxResource_, dxUAV_, 2, N, N);
		// u3: DzTexture
		CreateTextureUAV(device, dzResource_, dzUAV_, 3, N, N);
		// u4: PingTexture
		CreateTextureUAV(device, pingResource_, pingUAV_, 4, N, N);
		// u5: PongTexture
		CreateTextureUAV(device, pongResource_, pongUAV_, 5, N, N);
		// u6: DisplaceMap
		CreateTextureUAV(device, displaceResource_, displaceUAV_, 6, N, N);
		// u7: NormalFoamMap
		CreateTextureUAV(device, normalFoamResource_, normalFoamUAV_, 7, N, N);

		/// ===SRVインデックスの管理=== ///
		// CSのt0~t2として使用するため、3個の連続したデスクリプタを確保する
		uint32_t baseSRVIndex = srvManager_->AllocateContiguous(3);
		srvHeightIndex_ = baseSRVIndex;
		srvDxIndex_ = baseSRVIndex + 1;
		srvDzIndex_ = baseSRVIndex + 2;

		uint32_t baseSRVDisplace = srvManager_->AllocateContiguous(2);
		srvDisplaceIndex_ = baseSRVDisplace;      // t0
		srvNormalFoamIndex_ = baseSRVDisplace + 1;  // t1

		// srvHeightIndex（t0）
		CreateTextureSRV(srvHeightIndex_, pingResource_);
		// srvDxIndex（t1）
		CreateTextureSRV(srvDxIndex_, dxResource_);
		// srvDzIndex（t2）
		CreateTextureSRV(srvDzIndex_, dzResource_);
		// srvDisplaceIndex（t0:VS）
		CreateTextureSRV(srvDisplaceIndex_, displaceResource_);
		// srvNormalFoamIndex（t1:VS）
		CreateTextureSRV(srvNormalFoamIndex_, normalFoamResource_);

		// 初期値書き込み
		WriteOceanParams();
		preParams_ = params_;

		/// ===RippleSimulatorの初期化=== ///
		ripple_ = std::make_unique<RippleSimulator>();
		ripple_->Initialize(device, N, params_.gridWidth);
	}

	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void FFTOceanCompute::Update(float deltaTime) {
		// 経過時間の更新
		elapsedTime_ += deltaTime;
		params_.time = elapsedTime_;

		// rippleの更新処理
		ripple_->Update(deltaTime);

		// スペクトルに影響するパラメータが変更されたか検知
		if (params_.gridWidth != preParams_.gridWidth || 
			params_.windowSpeed != preParams_.windowSpeed ||
		    params_.windDirection.x != preParams_.windDirection.x || 
			params_.windDirection.y != preParams_.windDirection.y ||
		    params_.amplitude != preParams_.amplitude || 
			params_.gridSize != preParams_.gridSize ||
			params_.lambda != preParams_.lambda ||
			params_.foamThreshold != preParams_.foamThreshold) {

			// InitSpectrum を再実行させる
			isInitialized_ = false;
			preParams_ = params_;
		}

		WriteOceanParams();
	}

	///-------------------------------------------/// 
	/// ディスパッチ
	///-------------------------------------------///
	void FFTOceanCompute::Dispatch(ID3D12GraphicsCommandList* commandList) {
		assert(commandList);

		// SRVManagerからヒープを取得, 設定
		ID3D12DescriptorHeap* heaps[] = { srvManager_->GetDescriptorHeap() };
		commandList->SetDescriptorHeaps(1, heaps);

		const uint32_t N = params_.gridSize;
		const uint32_t threadGroupX = (N + kThreadGroupSize_ - 1) / kThreadGroupSize_;
		const uint32_t threadGroupY = threadGroupX;
		const uint32_t stages = static_cast<uint32_t>(std::log2(static_cast<float>(N)));

		auto CBV0 = oceanParamsBuffer_->GetBuffer()->GetGPUVirtualAddress();
		auto CBV1 = butterflyParamsBuffer_->GetBuffer()->GetGPUVirtualAddress();
		auto UAVTable = srvManager_->GetGPUDescriptorHandle(uavIndices_[0]);

		// =============================================
		//   InitSpectrum（初回のみ）
		//   h0(k) を H0Texture(u0) に書き込む
		// =============================================
		if (!isInitialized_) {
			Service::Render::SetCSPSO(commandList, CSPipelineType::FFTOcean, L"InitSpectrum");
			commandList->SetComputeRootConstantBufferView(0, CBV0);
			commandList->SetComputeRootDescriptorTable(2, UAVTable);
			commandList->Dispatch(threadGroupX, threadGroupY, 1);
			UAVBarrier(commandList, h0Resource_->GetBuffer());
			isInitialized_ = true;
		}

		// =============================================
		// Step 1: UpdateSpectrum（毎フレーム）
		//   HktTexture(u1) / DxTexture(u2) / DzTexture(u3) を更新
		// =============================================
		Service::Render::SetCSPSO(commandList, CSPipelineType::FFTOcean, L"UpdateSpectrum");
		commandList->SetComputeRootConstantBufferView(0, CBV0);
		commandList->SetComputeRootDescriptorTable(2, UAVTable);
		commandList->Dispatch(threadGroupX, threadGroupY, 1);
		UAVBarrier(commandList, hktResource_->GetBuffer());
		UAVBarrier(commandList, dxResource_->GetBuffer());
		UAVBarrier(commandList, dzResource_->GetBuffer());

		// =============================================
		//   IFFT（Height / Dx / Dz 共通の処理フロー）
		//   BitReverseRows → ButterflyIFFT(水平) → BitReverseCols → ButterflyIFFT(垂直)
		// =============================================
		for (uint32_t inputSource = 0; inputSource < 3; ++inputSource) {

			// --- BitReverseRows: inputSource → Ping(u4) ---
			WriteButterflyParams(0, 0, 0, inputSource);
			Service::Render::SetCSPSO(commandList, CSPipelineType::FFTOcean, L"BitReverseRows");
			commandList->SetComputeRootConstantBufferView(0, CBV0);
			commandList->SetComputeRootConstantBufferView(1, CBV1);
			commandList->SetComputeRootDescriptorTable(2, UAVTable);
			commandList->Dispatch(threadGroupX, threadGroupY, 1);
			UAVBarrier(commandList, pingResource_->GetBuffer());

			// --- ButterflyIFFT 水平方向 ---
			for (uint32_t s = 0; s < stages; ++s) {
				WriteButterflyParams(s, s % 2, 0, inputSource);
				Service::Render::SetCSPSO(commandList, CSPipelineType::FFTOcean, L"ButterflyIFFT");
				commandList->SetComputeRootConstantBufferView(0, CBV0);
				commandList->SetComputeRootConstantBufferView(1, CBV1);
				commandList->SetComputeRootDescriptorTable(2, UAVTable);
				commandList->Dispatch(threadGroupX, threadGroupY, 1);
				UAVBarrier(commandList, pingResource_->GetBuffer());
				UAVBarrier(commandList, pongResource_->GetBuffer());
			}

			// --- BitReverseCols: Pong(u5) → Ping(u4) ---
			WriteButterflyParams(0, 0, 0, inputSource);
			Service::Render::SetCSPSO(commandList, CSPipelineType::FFTOcean, L"BitReverseCols");
			commandList->SetComputeRootConstantBufferView(0, CBV0);
			commandList->SetComputeRootConstantBufferView(1, CBV1);
			commandList->SetComputeRootDescriptorTable(2, UAVTable);
			commandList->Dispatch(threadGroupX, threadGroupY, 1);
			UAVBarrier(commandList, pingResource_->GetBuffer());

			// --- ButterflyIFFT 垂直方向 ---
			for (uint32_t s = 0; s < stages; ++s) {
				WriteButterflyParams(s, s % 2, 1, inputSource);
				Service::Render::SetCSPSO(commandList, CSPipelineType::FFTOcean, L"ButterflyIFFT");
				commandList->SetComputeRootConstantBufferView(0, CBV0);
				commandList->SetComputeRootConstantBufferView(1, CBV1);
				commandList->SetComputeRootDescriptorTable(2, UAVTable);
				commandList->Dispatch(threadGroupX, threadGroupY, 1);
				UAVBarrier(commandList, pingResource_->GetBuffer());
				UAVBarrier(commandList, pongResource_->GetBuffer());
			}

			// IFFT結果を保存（inputSource=1:Dx, 2:Dz のみ実行、0:Heightは不要）
			if (inputSource != 0) {
				WriteButterflyParams(0, 0, 0, inputSource);
				Service::Render::SetCSPSO(commandList, CSPipelineType::FFTOcean, L"SaveIFFTResult");
				commandList->SetComputeRootConstantBufferView(0, CBV0);
				commandList->SetComputeRootConstantBufferView(1, CBV1);
				commandList->SetComputeRootDescriptorTable(2, UAVTable);
				commandList->Dispatch(threadGroupX, threadGroupY, 1);
				UAVBarrier(commandList, dxResource_->GetBuffer());
				UAVBarrier(commandList, dzResource_->GetBuffer());
			}
		}

		// =============================================
		//   RippleSimulator（波紋シミュレーション）
		//   FFT処理の後、AssembleDisplacementの前に実行
		// =============================================
		ripple_->Simulate(commandList);

		// =============================================
		//   AssembleDisplacement
		//   Ping/Pong の結果を DisplaceMap(u6) に合成
		// =============================================
		// バリアの遷移
		// Ping(Height) / Dx / Dz を UAV → SRV に遷移
		TransitionBarrier(commandList, pingResource_->GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		TransitionBarrier(commandList, dxResource_->GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		TransitionBarrier(commandList, dzResource_->GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		// RippleMap を SRV に遷移
		ripple_->TransitionToSRV(commandList);

		// AssembleDisplacement
		Service::Render::SetCSPSO(commandList, CSPipelineType::FFTOcean, L"AssembleDisplacement");
		commandList->SetComputeRootConstantBufferView(0, CBV0);
		commandList->SetComputeRootDescriptorTable(2, UAVTable);
		// t0〜t2 を DescriptorTable でセット
		commandList->SetComputeRootDescriptorTable(3, srvManager_->GetGPUDescriptorHandle(srvHeightIndex_));
		// t3
		commandList->SetComputeRootDescriptorTable(4, srvManager_->GetGPUDescriptorHandle(ripple_->GetCurrentSRVIndex()));
		commandList->Dispatch(threadGroupX, threadGroupY, 1);
		UAVBarrier(commandList, displaceResource_->GetBuffer());

		// =============================================
		//   ComputeNormalFoam
		//   法線 + 泡強度を NormalFoamMap(u7) に書き込む
		// =============================================
		Service::Render::SetCSPSO(commandList, CSPipelineType::FFTOcean, L"ComputeNormalFoam");
		commandList->SetComputeRootConstantBufferView(0, CBV0);
		commandList->SetComputeRootDescriptorTable(2, UAVTable);
		commandList->Dispatch(threadGroupX, threadGroupY, 1);
		UAVBarrier(commandList, normalFoamResource_->GetBuffer());

		// SRV → UAV に戻す（次フレームでまた書き込むため）
		TransitionBarrier(commandList, pingResource_->GetBuffer(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		TransitionBarrier(commandList, dxResource_->GetBuffer(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		TransitionBarrier(commandList, dzResource_->GetBuffer(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// RippleMap を UAV に戻す
		ripple_->TransitionToUAV(commandList);

		// DisplaceMap と NormalFoamMap を UAV → SRV に遷移（レンダリングで使用するため）
		TransitionBarrier(commandList, displaceResource_->GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		TransitionBarrier(commandList, normalFoamResource_->GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

	///-------------------------------------------/// 
	/// 次のフレームのためにリソース状態をリセットします 
	///-------------------------------------------///
	void FFTOceanCompute::ResetResourceStatesForNextFrame(ID3D12GraphicsCommandList* commandList) {
		// SRV->UAVに遷移
		TransitionBarrier(commandList, displaceResource_->GetBuffer(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		TransitionBarrier(commandList, normalFoamResource_->GetBuffer(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	///-------------------------------------------/// 
	/// テクスチャUAVリソースの生成
	///-------------------------------------------///
	void FFTOceanCompute::CreateTextureUAV(
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
		outResource->SetBuffer(resource.Get());

		// UAVの作成
		outUAV.CreateAsTexture2D(
			device,
			outResource->GetBuffer(),
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			srvManager_->GetCPUDescriptorHandle(uavIndices_[uavSlot])
		);
	}

	///-------------------------------------------/// 
	/// SRVリソースの生成
	///-------------------------------------------///
	void FFTOceanCompute::CreateTextureSRV(uint32_t index, std::unique_ptr<BufferBase>& outResource) {
		// SRVの生成処理を実装
		srvManager_->CreateSRVForTexture2D(
			index, 
			outResource->GetBuffer(),
			DXGI_FORMAT_R32G32B32A32_FLOAT, 
			1
		);
	}

	///-------------------------------------------/// 
	/// OceanParamsバッファの書き込み
	///-------------------------------------------///
	void FFTOceanCompute::WriteOceanParams() {
		if (oceanParamsData_) {
			*oceanParamsData_ = params_;
		}
	}

	///-------------------------------------------/// 
	/// ButterflyParamsバッファの書き込み
	///-------------------------------------------///
	void FFTOceanCompute::WriteButterflyParams(uint32_t stage, uint32_t pingPong, uint32_t direction, uint32_t inputSource) {
		if (butterflyParamsData_) {
			butterflyParamsData_->stages = stage;
			butterflyParamsData_->pingPong = pingPong;
			butterflyParamsData_->direction = direction;
			butterflyParamsData_->inputSource = inputSource;
		}
	}

	///-------------------------------------------/// 
	/// UAVバリア
	///-------------------------------------------///
	void FFTOceanCompute::UAVBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource) {
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		barrier.UAV.pResource = resource;
		commandList->ResourceBarrier(1, &barrier);
	}

	///-------------------------------------------/// 
	/// リソース状態の遷移バリア
	///-------------------------------------------///
	void FFTOceanCompute::TransitionBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
		D3D12_RESOURCE_BARRIER b = {};
		b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		b.Transition.pResource = resource;
		b.Transition.StateBefore = before;
		b.Transition.StateAfter = after;
		b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &b);
	}
}
