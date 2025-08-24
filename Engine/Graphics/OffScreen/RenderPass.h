#pragma once
#include "Engine/Graphics/OffScreen/RenderTexture.h"
#include <memory>

///=====================================================/// 
/// RenderPass 
///=====================================================///
class RenderPass {
public:
	RenderPass() = default;
	virtual ~RenderPass() = default;

	// 初期化
	virtual void Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> RenderTexture) = 0;
	// 描画
	virtual void PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);
	// 描画
	virtual void Draw(ID3D12GraphicsCommandList* commandList) = 0;
	// ImGui情報
	virtual void ImGuiInfo() {}

protected:
	std::shared_ptr<RenderTexture> renderTexture_;
};

 