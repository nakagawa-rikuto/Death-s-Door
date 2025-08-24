
#pragma once
/// ===Include=== ///
#include "Engine/Graphics/OffScreen/RenderPass.h"
// BufferBase
#include "Engine/Graphics/Base/BufferBase.h"

///=====================================================/// 
/// VignetteEffect
///=====================================================///
class VignetteEffect : RenderPass {
public:
	VignetteEffect() = default;
	~VignetteEffect() = default;
	// 初期化
	void Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> RenderTexture) override;
	// 描画
	void PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) override;
	// 描画
	void Draw(ID3D12GraphicsCommandList* commandList) override;
	// ImGui情報
	void ImGuiInfo() override;

public: /// ===Setter=== ///

	void SetVignetteData(float scale, float pawer);

private:

	// Buffer
	std::unique_ptr<BufferBase> buffer_;

	// / Dissolveエフェクトのデータ構造
	struct VignetteData {
		float scale; // discardの閾値
		float pawer; // smoothstep開始
		Vector2 padding; // floatの倍数にするためのパディング
	};
	VignetteData* data_ = nullptr;
};
