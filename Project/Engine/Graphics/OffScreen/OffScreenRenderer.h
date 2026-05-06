#pragma once
/// ===Include=== ///
// c++
#include <vector>
#include <unordered_map>
// Offscreen
#include "RenderTexture.h"
// RenderPass
#include "RenderPass.h"
// Data
#include "Engine/DataInfo/OffScreenData.h"

namespace MiiEngine {
	///=====================================================/// 
	/// OffScreenRenderer
	///=====================================================///
	class OffScreenRenderer {
	public:
		OffScreenRenderer() = default;
		~OffScreenRenderer();

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="device">リソース作成やコマンド実行に使用される ID3D12Device へのポインタ。</param>
		/// <param name="srv">シェーダーリソースビュー (SRV) を管理する SRVManager へのポインタ。</param>
		/// <param name="rtv">レンダーターゲットビュー (RTV) を管理する RTVManager へのポインタ。</param>
		/// <param name="width">初期化するレンダーターゲットの幅（ピクセル単位）。</param>
		/// <param name="height">初期化するレンダーターゲットの高さ（ピクセル単位）。</param>
		/// <param name="clearColor">レンダーターゲットをクリアする際に使用する色（Vector4、通常は RGBA）。</param>
		void Initialize(ID3D12Device* device, SRVManager* srv, RTVManager* rtv, uint32_t width, uint32_t height, const Vector4& clearColor);

		/// <summary>
		/// 描画前処理
		/// </summary>
		/// <param name="commandList">描画コマンドを記録する ID3D12GraphicsCommandList へのポインタ。操作対象の有効なコマンドリストを渡します。</param>
		/// <param name="dsvHandle">バインドする深度ステンシルビュー (DSV) を指す D3D12_CPU_DESCRIPTOR_HANDLE。</param>
		void PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

		/// <summary>
		/// 描画処理
		/// </summary>
		/// <param name="commandList">描画コマンドを記録・発行するための ID3D12GraphicsCommandList へのポインター。呼び出す前にコマンドリストが適切に初期化および設定されている必要があります。</param>
		void Draw(ID3D12GraphicsCommandList* commandList);

		/// <summary>
		/// 描画後処理
		/// </summary>
		/// <param name="commandList">後処理コマンドを記録するための ID3D12GraphicsCommandList へのポインター。</param>
		void PostDraw(ID3D12GraphicsCommandList* commandList);

		/// <summary>
		/// effectTexture を SwapChain にコピー
		/// </summary>
		/// <param name="commandList">コマンドリスト</param>
		/// <param name="swapChainRTV">SwapChain の RTV ハンドル</param>
		void CopyToSwapChain(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE swapChainRTV);

		/// <summary>
		/// エフェクトリストをクリア
		/// </summary>
		void ClearEffects();

		/// <summary>
		/// エフェクトリストを追加
		/// </summary>
		/// <param name="type">追加するエフェクトタイプ</param>
		void AddEffect(OffScreenType type);

		/// <summary>
		/// 特定のエフェクトをリストから削除
		/// </summary>
		/// <param name="type">削除するエフェクトタイプ</param>
		void RemoveEffect(OffScreenType type);

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
		ID3D12Resource* GetSceneBuffer() const;
		ID3D12Resource* GetEffectBuffer() const;

	public: /// ===Getter=== ///
		// 現在のRenderPassの取得
		RenderPass* GetRenderPass(OffScreenType type) const;
		// Pipelineのタイプの取得
		OffScreenType GetType();

	public: /// ===Setter=== ///
		// Typeの設定
		void SetType(OffScreenType type);

	private:
		// シーン描画用のRenderTexture
		std::shared_ptr<RenderTexture> sceneTexture_;
		// エフェクト適用後のRenderTexture
		std::shared_ptr<RenderTexture> pingPongTextures_[2];

		// RenderPassコンテナ
		std::unordered_map<OffScreenType, std::unique_ptr<RenderPass>> renderPass_;
		// 現在のRenderPass
		RenderPass* activePass_ = nullptr;

		// Pipelineのタイプ
		std::vector<OffScreenType> activeEffects_;

	private:
		/// <summary>
		/// 最終結果のテクスチャを取得
		/// </summary>
		/// <returns>最終結果のRenderTextureへの共有ポインタ</returns>
		std::shared_ptr<RenderTexture> GetFinalResultTexture() const;
	};
}

