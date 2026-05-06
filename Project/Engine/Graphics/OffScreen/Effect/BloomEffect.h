#pragma once
/// ===Include=== ///
#include "Engine/Graphics/OffScreen/RenderPass.h"
#include "Engine/Graphics/Base/BufferBase.h"


namespace MiiEngine {
	/// ===前方宣言=== ///
	class SRVManager;
	class RTVManager;

	/// ===Bloomエフェクトのデータ構造=== ///
	struct BloomData {
		float threshold;      // 輝度の閾値
		Vector3 brightness;   // RGB輝度の重み
		float intensity;      // ブルームの光の強さ
		Vector3 padding;      // パディング
	};

	///=====================================================/// 
	/// BloomEffect
	///=====================================================///
	class BloomEffect : public RenderPass {
	public:
		BloomEffect() = default;
		~BloomEffect() = default;

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="device">初期化に使用する D3D12 デバイスへのポインタ。GPU リソースの作成やコマンド発行に使用されます。</param>
		/// <param name="RenderTexture">初期化で使用するレンダーテクスチャへの std::shared_ptr。レンダリングの出力先や関連リソースを表します。</param>
		void Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> outputTexture) override;

		void PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) override;
		void Draw(ID3D12GraphicsCommandList* commandList) override;
		void ImGuiInfo() override;

		// Setter
		void SetData(BloomData data);

	private:
		std::unique_ptr<BufferBase> buffer_;
		BloomData* data_ = nullptr;

		// --- Bloom内部処理用のテクスチャ ---
		std::shared_ptr<RenderTexture> luminanceTexture_; // 輝度抽出結果用
		std::shared_ptr<RenderTexture> blurTexture_;      // ブラー結果用
	};
}