#pragma once
/// ===Include=== ///
// c++
#include <vector>
// Offscreen
#include "RenderTexture.h"
// Effect
#include "Effect/CopyImageEffect.h"
#include "Effect/GrayscaleEffect.h"
#include "Effect/VignetteEffect.h"
#include "Effect/OutLineEffect.h"
#include "Effect/BoxFilter3x3Effect.h"
#include "Effect/BoxFilter5x5Effect.h"
#include "Effect/RadiusBlurEffect.h"
#include "Effect/DissolveEffect.h"

// Data
#include "Engine/DataInfo/OffScreenTypeData.h"

///=====================================================/// 
/// OffScreenRenderer
///=====================================================///
class OffScreenRenderer {
public:
	OffScreenRenderer() = default;
	~OffScreenRenderer();

	// 初期化
	void Initialize(
		ID3D12Device* device, 
		SRVManager* srv, RTVManager* rtv, 
		uint32_t width, uint32_t height, const Vector4& clearColor);
	// 描画前処理
	void PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);
	// 描画処理
	void Draw(ID3D12GraphicsCommandList* commandList);
	// 描画後処理
	void PostDraw(ID3D12GraphicsCommandList* commandList);

#ifdef USE_IMGUI
	void DrawImGui();
#endif

public: /// ===Getter=== ///
	// RTVHandleの取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetResultRTV() const;
	// SRVの取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetResultSRV() const;
	// RTVのインデックス番号の取得
	uint32_t GetRTVHandleIndex() const;
	// SRVのインデックス番号の取得
	uint32_t GetSRVHandleIndex() const;
	// Resourceの取得
	ID3D12Resource* GetBuffer() const;

public: /// ===Setter=== ///
	// Typeの設定
	void SetType(OffScreenType type);

private:
	std::shared_ptr<RenderTexture> renderTexture_;

	// 各パス（あらかじめ生成済み）
	std::shared_ptr<CopyImageEffect> copyImage_;
	std::shared_ptr<GrayscaleEffect> grayscale_;
	std::shared_ptr<VignetteEffect> vignette_;
	std::shared_ptr<OutLineEffect> outLine_;
	std::shared_ptr<BoxFilter3x3Effect> boxFilter3x3_;
	std::shared_ptr<BoxFilter5x5Effect> boxFilter5x5_;
	std::shared_ptr<RadiusBlurEffect> radiusBlur_;
	std::shared_ptr<DissolveEffect> dissolve_;

	// Piplineのタイプ
	OffScreenType type_ = OffScreenType::CopyImage;
};

