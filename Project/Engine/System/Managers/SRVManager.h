#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Core/ComPtr.h"
// DirectX
#include <d3d12.h>
// c++
#include <cstdint>
#include <cassert>
#include <vector>

namespace MiiEngine {
	/// ===前方宣言=== ///
	class DXCommon;

	///=====================================================/// 
	/// SRVManager
	///=====================================================///
	class SRVManager {
	public:

		SRVManager() = default;
		~SRVManager();

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="dxcommon">初期化に使用する DXCommon オブジェクトへのポインター。有効なポインターを渡してください。</param>
		void Initialize(DXCommon* dxcommon);

		/// <summary>
		/// 描画処理の前に実行される準備処理を行う関数
		/// </summary>
		void PreDraw();

		/// <summary>
		/// SRV（シェーダーリソースビュー）の生成処理
		/// </summary>
		/// <param name="srvIndex">作成するSRVのインデックス（デスクリプタヒープやSRVスロット内の位置）。</param>
		/// <param name="pResource">SRVが参照するID3D12Resourceへのポインター。ビューの対象となるリソースを指定します。</param>
		/// <param name="desc">ビューの詳細を指定するD3D12_SHADER_RESOURCE_VIEW_DESC構造体（フォーマット、ミップ範囲、ビュータイプなど）。</param>
		void CreateShaderResourceView(uint32_t srvIndex, ID3D12Resource* pResource, D3D12_SHADER_RESOURCE_VIEW_DESC desc);

		/// <summary>
		/// リソースの割り当てを行い、割り当てられた識別子を返す
		/// </summary>
		/// <returns>割り当てられたリソースの識別子を表す 32 ビット符号なし整数 (uint32_t)。</returns>
		uint32_t Allocate();

		/// <summary>
		/// 指定した数の連続したリソースを割り当て、先頭のインデックスを返す
		/// </summary>
		/// <param name="count">連続して割り当てる数</param>
		/// <returns>割り当てられた連続領域の先頭のインデックス</returns>
		uint32_t AllocateContiguous(uint32_t count);

		/// <summary>
		/// SRVインデックスを解放して再利用可能にする
		/// </summary>
		/// <param name="srvIndex">解放するSRVインデックス</param>
		void Free(uint32_t srvIndex);

		/// <summary>
		/// メモリ割り当ての成否を検証
		/// </summary>
		/// <returns>アロケーションが成功した場合は true、失敗した場合は false を返します。</returns>
		bool AssertAllocate() const;

		/// <summary>
		/// 2D テクスチャ用の SRV（Shader Resource View）を作成
		/// </summary>
		/// <param name="srvIndex">SRV を作成するデスクリプタスロットのインデックス。</param>
		/// <param name="pResource">SRV を作成する対象の ID3D12Resource へのポインタ（2D テクスチャリソース）。</param>
		/// <param name="Format">SRV に使用するピクセルフォーマット (DXGI_FORMAT)。</param>
		/// <param name="MipLevels">使用するミップレベルの数。</param>
		void CreateSRVForTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

		/// <summary>
		/// 指定した D3D12 リソースに対してキューブマップ用の Shader Resource View (SRV) を作成
		/// </summary>
		/// <param name="srvIndex">SRV を作成する記述子ヒープ内のインデックス（配置先）を指定します。</param>
		/// <param name="pResource">キューブマップテクスチャを表す ID3D12Resource* へのポインタ。SRV を作成する元のリソースです。</param>
		/// <param name="Format">SRV に使用するピクセルフォーマット (DXGI_FORMAT)。ビューでリソースを解釈するフォーマットを指定します。</param>
		/// <param name="MipLevels">SRV に含めるミップマップレベルの数を指定します。</param>
		void CreateSRVForCubeMap(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

		/// <summary>
		/// 構造化バッファ（structured buffer）用の Shader Resource View (SRV) を作成
		/// </summary>
		/// <param name="srvIndex">SRV を格納するデスクリプタヒープ内のインデックス（または配置先のデスクリプタ位置）。</param>
		/// <param name="pResource">SRV を作成する対象の ID3D12Resource へのポインタ（構造化バッファを指す）。</param>
		/// <param name="numElements">バッファ内の要素（構造体）の数。</param>
		/// <param name="structureByteStride">各要素（構造体）のバイト単位のサイズ（ストライド）。</param>
		void CreateSRVForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	public:/// ===Setter=== ///

		// SRVインデックス開始番号の設定
		void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

	public:/// ===Getter=== ///
		// Heap
		ID3D12DescriptorHeap* GetDescriptorHeap() const;
		// CPU
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
		// GPU
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	public:/// ===定数=== ///

		// 最大SRV数（最大テクスチャ数）
		static const uint32_t kMaxSRVCount_;

	private:/// ===variable=== ///
		DXCommon* dXCommon_ = nullptr; // DXCommon

		// SRV用のでスクリプタサイズ
		uint32_t descriptorSize_;

		// SRV用でスクリプタヒープ
		ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		// 次に使用するSRVインデックス
		uint32_t useIndex_ = 0;

		// 再利用可能なSRVインデックスのリスト
		std::vector<uint32_t> freeIndices_;
	};
}


