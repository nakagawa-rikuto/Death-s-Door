#pragma once
/// ====Include== ///
// Engine
#include "Engine/Core/ComPtr.h"
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
// c++
#include <cstdlib>
#include <chrono>
#include <wrl.h>
#include <vector>

#pragma comment(lib, "dxcompiler.lib")

namespace MiiEngine {
	/// ===前方宣言=== ///
	class WinApp;
	class RTVManager;

	///=====================================================/// 
	/// DirectX汎用
	///=====================================================///
	class DXCommon {
	public:

		DXCommon() = default;
		~DXCommon();

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="winApp">初期化対象の WinApp オブジェクトへのポインタ。</param>
		/// <param name="backBufferWidth">バックバッファの幅（ピクセル単位）。</param>
		/// <param name="backBufferHeight">バックバッファの高さ（ピクセル単位）。</param>
		void Initialize(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight);

		/// <summary>
		/// RenderTextureへの描画前処理
		/// </summary>
		/// <param name="resource">準備対象のレンダーテクスチャを表す ID3D12Resource へのポインタ。nullptr を渡さないことが想定されます。</param>
		void PreDrawRenderTexture(ID3D12Resource* resource);

		/// <summary>
		/// バリアの状態遷移
		/// </summary>
		void TransitionRenderTarget();

		/// <summary>
		/// EffectTextureへの描画前処理
		/// </summary>
		/// <param name="resource">準備対象のエフェクトテクスチャを表す ID3D12Resource へのポインタ。</param>
		void PreDrawEffectTexture(ID3D12Resource* resource);

		/// <summary>
		/// EffectTextureのバリア状態遷移
		/// </summary>
		void TransitionEffectTexture();

		/// <summary>
		/// SwapChainへの描画前処理
		/// </summary>
		/// <param name="rtv">RTVManager 型へのポインタ。レンダーターゲットビュー（RTV）を管理するオブジェクトへの参照として使用される。</param>
		void PreDrawImGui(RTVManager* rtv);

		/// <summary>
		/// コマンドを積む
		/// </summary>
		void BeginCommand();



		/// <summary>
		/// 描画前処理
		/// </summary>
		void PostDraw();

		/// <summary>
		/// ディスクリプタヒープの生成
		/// </summary>
		/// <param name="heapType">作成するディスクリプタヒープの種類（D3D12_DESCRIPTOR_HEAP_TYPE）。</param>
		/// <param name="numDescriptors">ヒープに含めるディスクリプタの数（UINT）。</param>
		/// <param name="shaderVisible">true の場合、ヒープはシェーダーからアクセス可能（GPU 可視）になります。false の場合はシェーダーからはアクセスできません。</param>
		/// <returns>作成されたID3D12DescriptorHeapへのComPtr。作成に失敗した場合は空のComPtrが返される可能性があります。</returns>
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

		/// ===DescriptorSizeの取得=== ///
		const uint32_t GetRTVDescriptorSize(); // RTV
		const uint32_t GetDSVDescriptorSize(); // DSV
		const uint32_t GetSRVDescriptorSize(); // SRV

	public:/// ===Getter=== ///
		// DXGFactoryの取得
		IDXGIFactory7* GetDXGFactory()const;
		// デバイスの取得
		ID3D12Device* GetDevice()const;
		// DxcUtilsの取得
		IDxcUtils* GetDxcUtils()const;
		// DxcCompilerの取得
		IDxcCompiler3* GetDxcCompiler()const;
		// IncludeHandlerの取得
		IDxcIncludeHandler* GetIncludeHandler()const;
		// 描画コマンドリストの取得
		ID3D12GraphicsCommandList* GetCommandList()const;
		// BackVBufferIndexの取得
		UINT GetBackBufferIndex()const;
		// SwapChainResourceの取得
		ID3D12Resource* GetSwapChainResource(uint32_t index)const;
		// バックバッファの横幅の取得
		int32_t GetBackBufferWidth()const;
		// バックバッファの縦幅の取得
		int32_t GetBackBufferHeight()const;
		// バックバッファの数を取得
		size_t GetBackBufferCount()const;
		// FPSの取得
		float GetFPS()const;
		// CPUのディスクリプターハンドルの取得
		// <param name="descriptorHeap">ディスクリプタヒープへの参照。ID3D12DescriptorHeap の ComPtr。</param>
		// <param name="descriptorSize">ディスクリプタ 1 つ分のサイズ (バイト単位)。</param>
		// <param name="index">取得するディスクリプタのインデックス。</param>
		// <returns>指定したインデックスに対応する D3D12_CPU_DESCRIPTOR_HANDLE を返します。</returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
			const ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
		// GPUのディスクリプターハンドルの取得
		// <param name="descriptorHeap">ディスクリプタヒープへの参照。ID3D12DescriptorHeap の ComPtr。</param>
		// <param name="descriptorSize">ディスクリプタ 1 つ分のサイズ (バイト単位)。</param>
		// <param name="index">取得するディスクリプタのインデックス。</param>
		// <returns>指定したインデックスに対応する D3D12_GPU_DESCRIPTOR_HANDLE を返します。</returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
			const ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	private: // メンバ変数

		// ウィンドウズアプリケーション管理
		WinApp* winApp_;

		/* //////////////////
			　Direct3D関連
		*/ //////////////////
		ComPtr<IDXGIFactory7> dxgiFactory_; // DXGIFactory
		ComPtr<ID3D12Device> device_; // D3D12Device
		ComPtr<IDxcUtils> dxcUtils_; // DxcUtils
		ComPtr<IDxcCompiler3> dxcCompiler_; // DxcCompiler
		ComPtr<IDxcIncludeHandler> includeHandler_; // includeHandler

		/// ===command=== ///
		ComPtr<ID3D12GraphicsCommandList> commandList_; // CommandList
		ComPtr<ID3D12CommandAllocator> commandAllocator_; // CommandAllocator
		ComPtr<ID3D12CommandQueue> commandQueue_; // CommandQueue

		/// ===swapChain=== ///
		ComPtr<IDXGISwapChain4> swapChain_; // SwapChain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
		ComPtr<ID3D12Resource> swapChainResource_[2];

		/// ===backBuffer=== ///
		std::vector<ComPtr<ID3D12Resource>> backBuffers_; // BackBuffer

		/// ===fence=== ///
		ComPtr<ID3D12Fence> fence_; // Fence
		uint64_t fenceValue_ = 0;  // FenceValue

		/// ===バリア=== ///
		D3D12_RESOURCE_BARRIER barrierRenderTexture_{};
		D3D12_RESOURCE_BARRIER barrierEffectTexture_{};
		D3D12_RESOURCE_BARRIER barrierSwapChain_{};

		/// ===FPS固定=== ///
		std::chrono::steady_clock::time_point reference_; // 記録時間(FPS固定用)
		float fps_ = 0.0f; // 現在のFPS

		/// ===viewPort=== ///
		D3D12_VIEWPORT viewPort_; // ビューポート

		/// ===scissor=== ///
		D3D12_RECT scissorRect_; // シザー矩形

		/// ===backBufferSize=== ///
		int32_t backBufferWidth_ = 0;
		int32_t backBufferHeight_ = 0;

	private:/// ===関数=== ///

		/// <summary>
		/// デバッグレイヤー
		/// </summary>
		void DebugLayer();

		/// <summary>
		/// エラー・警告
		/// </summary>
		void DebugInfo();

		/// <summary>
		/// DXGIデバイス初期化処理
		/// </summary>
		void InitializeDXGIDevice();

		/// <summary>
		/// コマンド関連の初期化処理
		/// </summary>
		void InitializeCommand();

		/// <summary>
		/// スワップチェーンの生成処理
		/// </summary>
		void CreateSwapChain();

		/// <summary>
		/// フェンスの生成処理
		/// </summary>
		void CreateFence();

		/// <summary>
		/// DXCの初期化処理
		/// </summary>
		void InitializeCompiler();

		/// <summary>
		/// ビューポートの生成処理
		/// </summary>
		/// <param name="kClientWidth">クライアント領域の幅（ピクセル）。</param>
		/// <param name="kClientHeight">クライアント領域の高さ（ピクセル）。</param>
		void CreateViewport(const int32_t kClientWidth, const int32_t kClientHeight);

		/// <summary>
		/// シザー矩形の生成処理
		/// </summary>
		/// <param name="kClientWidth">クライアント領域の幅（int32_t、ピクセル単位）。関数内で使用される読み取り専用の値です。</param>
		/// <param name="kClientHeight">クライアント領域の高さ（int32_t、ピクセル単位）。関数内で使用される読み取り専用の値です。</param>
		void CreateScissor(const int32_t kClientWidth, const int32_t kClientHeight);

		/// <summary>
		/// FPS固定の初期化処理
		/// </summary>
		void InitializeFixFPS();

		/// <summary>
		/// FPS固定の更新処理
		/// </summary>
		void UpdateFixFPS();
	};
}
