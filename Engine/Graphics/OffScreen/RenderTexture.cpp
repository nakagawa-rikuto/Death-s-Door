#include "RenderTexture.h"
// Engine
#include "Engine/System/Managers/RTVManager.h"
#include "Engine/System/Managers/DSVManager.h"
#include "Engine/System/Managers/SRVManager.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
RenderTexture::~RenderTexture() {
	buffer_.Reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void RenderTexture::Initialize(
	SRVManager* srv, RTVManager* rtv,
	uint32_t width, uint32_t height, const Vector4& clearColor, DXGI_FORMAT format) {

	// Managerの取得
	rtvManager_ = rtv;
	srvManager_ = srv;

	// 値の代入
	width_ = width;
	height_ = height;
	format_ = format;
	clearColor_ = clearColor;
}

///-------------------------------------------///  
/// RenderTextureの生成
///-------------------------------------------///
void RenderTexture::CreateRenderTexture(ID3D12Device* device) {
	/// ===Resource=== ///
	// DESCの設定
	D3D12_RESOURCE_DESC textureDesc{};
	textureDesc.Width = width_;
	textureDesc.Height = height_;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = format_;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // RenderTextureとして利用可能にする
	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // 当然VRAM状に作る
	// ClearValueの設定
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format_; // フォーマット
	clearValue.Color[0] = clearColor_.x; // RGBAの値
	clearValue.Color[1] = clearColor_.y;
	clearValue.Color[2] = clearColor_.z;
	clearValue.Color[3] = clearColor_.w;
	// Resourceの生成
	device->CreateCommittedResource(
		&heapProperties,			// Heapの設定
		D3D12_HEAP_FLAG_NONE,		// Heapのフラグ
		&textureDesc,				// ResourceのDESC
		D3D12_RESOURCE_STATE_RENDER_TARGET, // Resourceの状態
		&clearValue,				// ClearValueの設定
		IID_PPV_ARGS(&buffer_));	// Resourceのポインタ


	/// ===RTV=== ///
	// DESCの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = format_; // フォーマット
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	// RTVの作成
	rtvHandleIndex_ = rtvManager_->Allocate(); // RTVのインデックスを確保
	rtvManager_->CreateRenderTarget(rtvHandleIndex_, buffer_.Get(), rtvDesc);

	/// ===SRV=== ///
	// DESCの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = format_; // フォーマット
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// SRVの作成
	srvHandleIndex_ = srvManager_->Allocate(); // SRVのインデックスを確保
	srvManager_->CreateSRV(srvHandleIndex_, buffer_.Get(), srvDesc);
}

///-------------------------------------------/// 
/// クリア
///-------------------------------------------///
void RenderTexture::Clear(ID3D12GraphicsCommandList* commandList) {
	// RTVのクリア
	const float clearColor[4] = { clearColor_.x, clearColor_.y, clearColor_.z, clearColor_.w };
	rtvManager_->ClearRenderTarget(commandList, rtvHandleIndex_, clearColor);
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// RenderTextureの取得
ID3D12Resource* RenderTexture::GetBuffer() { return buffer_.Get(); }
// RTVHandleの取得
D3D12_CPU_DESCRIPTOR_HANDLE RenderTexture::GetRTVHandle() { return rtvManager_->GetCPUDescriptorHandle(rtvHandleIndex_); }
// SRVHandleの取得
D3D12_GPU_DESCRIPTOR_HANDLE RenderTexture::GetSRVHandle() { return srvManager_->GetGPUDescriptorHandle(srvHandleIndex_); }
// RTVのインデックス番号の取得
uint32_t RenderTexture::GetRTVHandleIndex() const { return rtvHandleIndex_; }
// SRVのインデックス番号の取得
uint32_t RenderTexture::GetSRVHandleIndex() const { return srvHandleIndex_; }
