#pragma once
/// ===include=== ///
#include "Engine/Graphics/Base/BufferBase.h"
#include "Engine/Graphics/Base/UAV.h"
#include "Engine/Graphics/Pipeline/CS/CSPSOCommon.h"
#include "Engine/DataInfo/OceanData.h"
// c++
#include <memory>
#include <d3d12.h>
#include <wrl.h>

namespace MiiEngine {
	/// ===前方宣言=== ///
	class SRVManager;

	///=====================================================/// 
	/// FFTOceanCompute
	///=====================================================///
	class FFTOceanCompute {
	public:

		FFTOceanCompute() = default;
		~FFTOceanCompute();

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="device">使用するID3D12Deviceへのポインタ。</param>
		/// <param name="gridSize">グリッドサイズ。</param>
		void Initialize(ID3D12Device* device, int gridSize);

		/// <summary>
		/// 更新処理（OceanParamsの時間等を更新）
		/// </summary>
		/// <param name="deltaTime">前フレームからの経過時間。</param>
		void Update(float deltaTime);

		/// <summary>
		/// コンピュートシェーダーのディスパッチ処理
		/// </summary>
		/// <param name="commandList">使用するコマンドリスト。</param>
		void Dispatch(ID3D12GraphicsCommandList* commandList);

		/// <summary>
		/// 次のフレームのためにリソース状態をリセットします。
		/// </summary>
		/// <param name="commandList">リソース状態の遷移を記録するために使用されるDirect3D 12グラフィックスコマンドリスト。</param>
		void ResetResourceStatesForNextFrame(ID3D12GraphicsCommandList* commandList);



	public: /// ===Getter=== ///
		// 最終変位マップ（SRVとして使用）
		ID3D12Resource* GetDisplacementMap() const;
		// 法線・泡マップ（SRVとして使用）
		ID3D12Resource* GetNormalFoamMap() const;
		// OceanParamsの取得
		OceanParams& GetOceanParams();
		// SRVインデックスの取得
		uint32_t GetSRVDisplaceIndex() const;
		uint32_t GetSRVNormalFoamIndex() const;

	public: /// ===Setter=== ///

		// OceanParamsの設定
		void SetOceanParams(const OceanParams& params);
		// グリッドサイズの設定
		void SetGridSize(uint32_t gridSize);

	private:

		/// ===定数=== ///
		static constexpr uint32_t kDefaultGridSize_ = 256;
		static constexpr uint32_t kThreadGroupSize_ = 16;

		/// ===経過時間=== ///
		float elapsedTime_ = 0.0f;
		bool isInitialized_ = false;

		/// ===SRVManager=== ///
		SRVManager* srvManager_ = nullptr;

		/// ===OceanParams定数バッファ=== ///
		std::unique_ptr<BufferBase> oceanParamsBuffer_;
		OceanParams* oceanParamsData_ = nullptr;

		/// ===ButterflyParams定数バッファ=== ///
		std::unique_ptr<BufferBase> butterflyParamsBuffer_;
		ButterflyParams* butterflyParamsData_ = nullptr;

		/// ===UAVテクスチャリソース（u0〜u7）=== ///
		//  u0: H0Texture      初期スペクトル
		std::unique_ptr<BufferBase> h0Resource_;
		UAV h0UAV_;
		//  u1: HktTexture     時間更新スペクトル（高さ）
		std::unique_ptr<BufferBase> hktResource_;
		UAV hktUAV_;
		//  u2: DxTexture      水平変位スペクトル X
		std::unique_ptr<BufferBase> dxResource_;
		UAV dxUAV_;
		//  u3: DzTexture      水平変位スペクトル Z
		std::unique_ptr<BufferBase> dzResource_;
		UAV dzUAV_;
		//  u4: PingTexture    IFFT Ping バッファ
		std::unique_ptr<BufferBase> pingResource_;
		UAV pingUAV_;
		//  u5: PongTexture    IFFT Pong バッファ
		std::unique_ptr<BufferBase> pongResource_;
		UAV pongUAV_;
		//  u6: DisplaceMap    最終変位マップ
		std::unique_ptr<BufferBase> displaceResource_;
		UAV displaceUAV_;
		//  u7: NormalFoamMap  法線 + 泡強度
		std::unique_ptr<BufferBase> normalFoamResource_;
		UAV normalFoamUAV_;

		// UAVインデックスを管理
		uint32_t uavIndices_[8] = {};

		// SRVインデックスを管理
		uint32_t srvHeightIndex_ = 0;
		uint32_t srvDxIndex_ = 0;
		uint32_t srvDzIndex_ = 0;
		uint32_t srvDisplaceIndex_ = 0;
		uint32_t srvNormalFoamIndex_ = 0;

		/// ===OceanParams初期値=== ///
		OceanParams params_ = {
			kDefaultGridSize_,   // gridSize
			1000.0f,             // gridWidth
			0.0f,                // time
			10.0f,               // windowSpeed
			{ 1.0f, 0.0f },      // windDirection
			1.0f,                // amplitude
			1.0f,                // lambda
			0.0f                 // foamThreshold
		};

		OceanParams preParams_;

	private:

		/// <summary>
		/// テクスチャUAVリソースの生成
		/// </summary>
		void CreateTextureUAV(
			ID3D12Device* device,
			std::unique_ptr<BufferBase>& outResource,
			UAV& outUAV,
			int uavSlot,
			uint32_t width, uint32_t height,
			DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);

		/// <summary>
		/// SRVリソースの生成
		/// </summary>
		/// <param name="index">SRVインデックス。</param>
		void CreateTextureSRV(
			uint32_t index, 
			std::unique_ptr<BufferBase>& outResource);

		/// <summary>
		/// OceanParamsバッファの書き込み
		/// </summary>
		void WriteOceanParams();

		/// <summary>
		/// ButterflyParamsバッファの書き込み
		/// </summary>
		void WriteButterflyParams(uint32_t stage, uint32_t pingPong, uint32_t direction, uint32_t inputSource);

		/// <summary>
		/// UAVバリア
		/// </summary>
		void UAVBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource);
	};
}


