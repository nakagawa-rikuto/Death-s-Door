#pragma once
// ComPtr
#include "Engine/Core/ComPtr.h"
// Vector4
#include "Math/Vector4.h"
// DirectX
#include <d3d12.h>
// uint32_t
#include <cstdint>

namespace MiiEngine {
	/// ===前方宣言=== ///
	class SRVManager;
	class RTVManager;

	///=====================================================/// 
	/// RenderTexture
	///=====================================================///
	class RenderTexture {
	public:

		RenderTexture() = default;
		~RenderTexture();

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="srv">SRVManager のポインタ。シェーダーリソースビュー（SRV）を管理するオブジェクト。</param>
		/// <param name="rtv">RTVManager のポインタ。レンダーターゲットビュー（RTV）を管理するオブジェクト。</param>
		/// <param name="width">初期化するターゲットの幅（ピクセル単位）。</param>
		/// <param name="height">初期化するターゲットの高さ（ピクセル単位）。</param>
		/// <param name="clearColor">レンダーターゲットをクリアする際に使用する色。Vector4 で RGBA を表します。</param>
		/// <param name="format">使用するピクセルフォーマットを示す DXGI_FORMAT 値。</param>
		void Initialize(SRVManager* srv, RTVManager* rtv, uint32_t width, uint32_t height, const Vector4& clearColor, DXGI_FORMAT format);

		/// <summary>
		/// RenderTextureの生成処理
		/// </summary>
		/// <param name="device">レンダーテクスチャの作成に使用する ID3D12Device へのポインター。nullptr であってはなりません。</param>
		void CreateRenderTexture(ID3D12Device* device);

		/// <summary>
		/// RenderTextureのクリア
		/// </summary>
		/// <param name="commandList">クリア操作を記録するための ID3D12GraphicsCommandList へのポインター。有効な（nullptr でない）コマンドリストを渡してください。</param>
		void Clear(ID3D12GraphicsCommandList* commandList);

	public: /// ===Getter=== ///

		// RenderTextureの取得
		ID3D12Resource* GetBuffer();
		// RTVManagerの取得
		RTVManager* GetRTVManager();
		// SRVManagerの取得
		SRVManager* GetSRVManager();
		// RTVHandleの取得
		D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle();
		// SRVHandleの取得
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle();
		// RTVのインデックス番号の取得
		uint32_t GetRTVHandleIndex() const;
		// SRVのインデックス番号の取得
		uint32_t GetSRVHandleIndex() const;
		// サイズ
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

	private:
		// Manager
		RTVManager* rtvManager_ = nullptr;
		SRVManager* srvManager_ = nullptr;

		// Info
		uint32_t width_;
		uint32_t height_;
		DXGI_FORMAT format_;
		Vector4 clearColor_;

		// Resource
		ComPtr<ID3D12Resource> buffer_;
		uint32_t rtvHandleIndex_ = 0;
		uint32_t srvHandleIndex_ = 0;
	};
}

