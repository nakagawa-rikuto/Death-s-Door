#pragma once
/// ===Include=== ///
// BufferBase
#include "Engine/Graphics/Base/BufferBase.h"
// RenderPass
#include "Engine/Graphics/OffScreen/RenderPass.h"
// c++
#include <string>
// Math
#include "Math/Vector3.h"

///=====================================================/// 
/// CopyImageEffect
///=====================================================///
class DissolveEffect : public RenderPass {
public:
	DissolveEffect() = default;
	~DissolveEffect() = default;

	// 初期化
	void Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> RenderTexture) override;
	// 描画
	void PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) override;
	// 描画
	void Draw(ID3D12GraphicsCommandList* commandList) override;
	// ImGui情報
	void ImGuiInfo() override;

public: /// ===Setter=== ///

	void SetDissolveData(float threshold, float edgeStart, float edgeEnd, const Vector3& edgeColor);
	void SetTexture(std::string& texuteKeyName);

private:

	// テクスチャ名
	std::string textureKeyName_ = "Dissolve";

	// Buffer
	std::unique_ptr<BufferBase> buffer_;

	// / Dissolveエフェクトのデータ構造
	struct DissolveData {
		float threshold; // discardの閾値
		float edgeStart; // smoothstep開始
		float edgeEnd; // smoothstep終了
		Vector3 edgeColor; // エッジ色
		float padding; // floatの倍数にするためのパディング
	};
	DissolveData* data_ = nullptr;
};