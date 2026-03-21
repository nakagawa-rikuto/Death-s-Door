#include "FFTOceanBase.h"
// CameraCommon
#include "Engine/Camera/Base/CameraCommon.h"
// SRVManager
#include "Engine/System/Managers/SRVManager.h"
// Service
#include "Service/Locator.h"
#include "Service/Render.h"
// C++
#include <cassert>

namespace MiiEngine {
	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	FFTOceanBase::~FFTOceanBase() {
		transformBuffer_.reset();
		oceanBuffer_.reset();
	}

	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	void FFTOceanBase::Initialize(ID3D12Device* device) {
		assert(device);

		srvManager_ = Service::Locator::GetSRVManager();

		// OceanRenderCBバッファ生成
		oceanBuffer_ = std::make_unique<BufferBase>();
		oceanBuffer_->Create(device, sizeof(OceanRenderCB));

		void* oceanMapped = nullptr;
		oceanBuffer_->GetBuffer()->Map(0, nullptr, &oceanMapped);
		oceanRenderCBData_ = static_cast<OceanRenderCB*>(oceanMapped);

		// TransformCBバッファ生成
		transformBuffer_ = std::make_unique<BufferBase>();
		transformBuffer_->Create(device, sizeof(TransformCB));

		void* transformMapped = nullptr;
		transformBuffer_->GetBuffer()->Map(0, nullptr, &transformMapped);
		transformCBData_ = static_cast<TransformCB*>(transformMapped);

		// 初期値書き込み
		WriteOceanRenderCB();
	}

	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void FFTOceanBase::Update() {
		WriteOceanRenderCB();
		WriteTransformCB();
	}

	///-------------------------------------------/// 
	/// バインド
	///-------------------------------------------///
	void FFTOceanBase::Bind(ID3D12GraphicsCommandList* commandList) {
		assert(commandList);
		assert(oceanBuffer_);
		assert(transformBuffer_);

		// SRVManagerからヒープを取得, 設定
		ID3D12DescriptorHeap* heaps[] = { srvManager_->GetDescriptorHeap() };
		commandList->SetDescriptorHeaps(1, heaps);

		// TransformCB（VS）
		commandList->SetGraphicsRootConstantBufferView(0, transformBuffer_->GetBuffer()->GetGPUVirtualAddress());
		// OceanRenderCB（PS）
		commandList->SetGraphicsRootConstantBufferView(1, oceanBuffer_->GetBuffer()->GetGPUVirtualAddress());
		// テクスチャ３つ(t0~t1)は、FFTOceanRendererでバインド
		// [2] t0:DisplaceMap, t1:NormalFoamMap（CSの出力をSRVとして渡す）
		commandList->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(srvDisplaceIndex_));
		// [3] t2:FoamTexture（通常テクスチャ）
		Service::Render::SetGraphicsRootDescriptorTable(commandList, 3, foamTextureName_);
	}

	///-------------------------------------------/// 
	/// Getter
	///-------------------------------------------///
	// OceanRenderCBの取得
	OceanRenderCB& FFTOceanBase::GetOceanRenderCB() {return oceanRenderCB_;}
	// TileScaleの取得
	float FFTOceanBase::GetTileScale() const {return tileScale_;}
	// Sizeの取得
	float FFTOceanBase::GetSize() const { return size_; }

	///-------------------------------------------/// 
	/// Setter
	///-------------------------------------------///
	// Cameraの設定
	void FFTOceanBase::SetCamera(CameraCommon* camera) {camera_ = camera;}
	// OceanRenderCBの設定
	void FFTOceanBase::SetOceanRenderCB(const OceanRenderCB& data) {oceanRenderCB_ = data;}
	// ワールド行列の設定
	void FFTOceanBase::SetWorldMatrix(const Matrix4x4& world) {worldMatrix_ = world;}
	// タイルスケールの設定
	void FFTOceanBase::SetTileScale(float tileScale) {tileScale_ = tileScale;}
	// サイズの設定
	void FFTOceanBase::SetSize(float size) {size_ = size;}
	// CS側のSRVインデックスを受け取る
	void FFTOceanBase::SetDisplaceSRVIndex(uint32_t index) { srvDisplaceIndex_ = index; }
	// CS側のSRVインデックスを受け取る
	void FFTOceanBase::SetNormalFoamSRVIndex(uint32_t index) { srvNormalFoamIndex_ = index; }
	// 泡テクスチャ名の設定
	void FFTOceanBase::SetFoamTextureName(const std::string& name) { foamTextureName_ = name; }

	///-------------------------------------------/// 
	/// OceanRenderCBの書き込み処理
	///-------------------------------------------///
	void FFTOceanBase::WriteOceanRenderCB() {
		if (oceanRenderCBData_) {
			*oceanRenderCBData_ = oceanRenderCB_;
		}
	}

	///-------------------------------------------/// 
	/// TransformCBの書き込み処理
	///-------------------------------------------///
	void FFTOceanBase::WriteTransformCB() {
		if (!transformCBData_ || !camera_) {
			return;
		}
		transformCBData_->world = worldMatrix_;
		transformCBData_->view = camera_->GetViewMatrix();
		transformCBData_->projection = camera_->GetProjectionMatrix();
		transformCBData_->cameraPos = camera_->GetTranslate();
		transformCBData_->size = size_;
		transformCBData_->tileScale = tileScale_;
	}
}
