#pragma once
/// ===Include=== ///
#include "Engine/Graphics/OffScreen/RenderPass.h"
// BufferBase
#include "Engine/Graphics/Base/BufferBase.h"
// Math
#include "Math/Vector2.h"

///=====================================================/// 
/// RadiusBlurEffect
///=====================================================///
class RadiusBlurEffect : RenderPass {
public:
	RadiusBlurEffect() = default;
	~RadiusBlurEffect() = default;
	// 初期化
	void Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> RenderTexture) override;
	// 描画
	void PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) override;
	// 描画
	void Draw(ID3D12GraphicsCommandList* commandList) override;
	// ImGui情報
	void ImGuiInfo() override;

public: /// ===Setter=== ///

	void SetRadiusBlurData(const Vector2& center, int numSamples, float blurWidth);

private:

	// Buffer
	std::unique_ptr<BufferBase> buffer_;

	// / Dissolveエフェクトのデータ構造
	struct RadiusBlurData {
		Vector2 center;  // ブラーの中心座標
		int numSamples;  // サンプリング数
		float blurWidth; // ブラーの幅
		float padding;   // floatの倍数にするためのパディング
	};
	RadiusBlurData* data_ = nullptr;
};