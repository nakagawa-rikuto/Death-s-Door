#pragma once
/// ===Include=== ///
// BufferBase
#include "Engine/Graphics/Base/BufferBase.h"
// RenderPass
#include "Engine/Graphics/OffScreen/RenderPass.h"

/// ===ShatterGlassエフェクトのデータ構造=== ///
struct ShatterGlassData {
	float progress;      // 割れる進行度 (0.0 ~ 1.0)
	float impactX;       // 衝撃点のX座標 (0.0 ~ 1.0)
	float impactY;       // 衝撃点のY座標 (0.0 ~ 1.0)
	float crackDensity;  // ひび割れの密度
	float dispersion;    // 破片の飛散度
	float rotation;      // 破片の回転量
	float fadeOut;       // フェードアウト効果
	float padding;       // パディング
};

///=====================================================/// 
/// ShatterGlassEffect
///=====================================================///
class ShatterGlassEffect : public RenderPass {
public:
	ShatterGlassEffect() = default;
	~ShatterGlassEffect() = default;

	// 初期化
	void Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> RenderTexture) override;
	// 描画前処理
	void PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) override;
	// 描画
	void Draw(ID3D12GraphicsCommandList* commandList) override;
	// ImGui情報
	void ImGuiInfo() override;

public:
	// データ
	void SetData(ShatterGlassData data);

private:
	// Buffer
	std::unique_ptr<BufferBase> buffer_;

	// Data
	ShatterGlassData* data_ = nullptr;
};