#pragma once
/// ===Include=== ///
#include "Engine/Graphics/OffScreen/RenderPass.h"

///=====================================================/// 
/// CopyImageEffect
///=====================================================///
class CopyImageEffect : public RenderPass {
public:
	CopyImageEffect() = default;
	~CopyImageEffect() = default;

	// 初期化
	void Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> RenderTexture) override;
	// 描画
	void PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) override;
	// 描画
	void Draw(ID3D12GraphicsCommandList* commandList) override;
	// ImGui情報
	void ImGuiInfo() override;
};