#pragma once
/// ===Incudle=== ///
#include <Engine/Graphics/Base/BufferBase.h>
#include <Engine/Graphics/Base/UAV.h>
#include <Engine/DataInfo/OceanData.h>
// c++
#include <memory>
#include <d3d12.h>
#include <wrl.h>

namespace MiiEngine {

	class SRVManager;

	///=====================================================/// 
	/// 波紋シュミレーター
	///=====================================================///
	class RippleSimulator {
	public:
		RippleSimulator() = default;
		~RippleSimulator();

		/// <summary>
		/// グリッドを初期化します。
		/// </summary>
		/// <param name="device">初期化に使用するDirect3D 12デバイス。</param>
		/// <param name="gridSize">グリッドのサイズ。</param>
		/// <param name="gridWidth">グリッドの幅。</param>
		void Initialize(ID3D12Device* device, uint32_t gridSize, float gridWidth);

		/// <summary>
		/// 状態を更新します。
		/// </summary>
		/// <param name="deltaTime">前回の更新からの経過時間（秒単位）。</param>
		void Update(float deltaTime);

		/// <summary>
		/// 波紋シミュレーション
		/// </summary>
		void Simulate(ID3D12GraphicsCommandList* commandList);

		/// <summary>
		/// 指定UV座標に波紋を注入する(複数対応)
		/// </summary>
		/// <param name="commandList">コマンドリスト</param>
		/// <param name="ripples">波紋情報のリスト</param>
		void AddRipples(
			ID3D12GraphicsCommandList* commandList,
			const std::vector<RippleInjection>& ripples);

		/// <summary>
		/// リソースをシェーダー リソース ビュー (SRV) 状態に遷移します。
		/// </summary>
		/// <param name="commandList">遷移を記録するコマンド リスト。</param>
		void TransitionToSRV(ID3D12GraphicsCommandList* commandList);

		/// <summary>
		/// リソースをUAV状態に遷移します。
		/// </summary>
		/// <param name="commandList">遷移コマンドを記録するコマンドリスト。</param>
		void TransitionToUAV(ID3D12GraphicsCommandList* commandList);

	public: /// ===Getter=== ///
		/// <summary>
		/// 現在のRippleバッファのSRVインデックスを返す
		/// </summary>
		uint32_t GetCurrentSRVIndex() const;
		// 波紋の速度を取得
		float GetWaveSpeed() const { return params_.waveSpeed; }
		// 波紋の減衰率を取得
		float GetDamping() const { return params_.damping; }
		// 出現位置のUV座標を取得
		Vector2 GetInjectionUV() const { return injectionArray_.injections[0].uv; }


	public: /// ===Setter=== ///
		// 波紋の速度を設定
		void SetWaveSpeed(float speed) { params_.waveSpeed = speed; }
		// 波紋の減衰率を設定
		void SetDamping(float damping) { params_.damping = damping; }
		// 出現位置のUV座標を設定
		void SetInjectionUV(const Vector2& uv) { injectionArray_.injections[0].uv = uv; }

	private:
		// スレッドグループサイズ
		static constexpr uint32_t kThreadGroupSize_ = 8;

		uint32_t gridSize_ = 256;
		float    gridWidth_ = 1000.0f;
		uint32_t pingPong_ = 0;

		// SRV管理クラスへのポインタ
		SRVManager* srvManager_ = nullptr;

		// 定数バッファ
		std::unique_ptr<BufferBase> paramsBuffer_;
		RippleParams* paramsData_ = nullptr;

		std::unique_ptr<BufferBase> injectionBuffer_;
		RippleInjectionArray* injectionData_ = nullptr;

		// Ping/Pong テクスチ
		std::unique_ptr<BufferBase> pingResource_;
		UAV pingUAV_;
		std::unique_ptr<BufferBase> pongResource_;
		UAV pongUAV_;

		uint32_t uavIndices_[2] = {};
		uint32_t srvPingIndex_ = 0;
		uint32_t srvPongIndex_ = 0;

		// 波紋シミュレーションのパラメータ初期化
		RippleParams params_{};
		RippleInjectionArray injectionArray_{};

	private:

		/// <summary>
		/// テクスチャUAV（アンオーダードアクセスビュー）を作成します。
		/// </summary>
		/// <param name="device">Direct3D 12デバイスへのポインタ。</param>
		/// <param name="outResource">作成されたバッファリソースの出力パラメータ。</param>
		/// <param name="outUAV">作成されたUAVの出力パラメータ。</param>
		/// <param name="uavSlot">UAVのスロットインデックス。</param>
		/// <param name="width">テクスチャの幅。</param>
		/// <param name="height">テクスチャの高さ。</param>
		/// <param name="format">テクスチャのピクセルフォーマット（デフォルトはDXGI_FORMAT_R32G32B32A32_FLOAT）。</param>
		void CreateTextureUAV(
			ID3D12Device* device,
			std::unique_ptr<BufferBase>& outResource,
			UAV& outUAV,
			int uavSlot,
			uint32_t width, uint32_t height,
			DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);

		/// <summary>
		/// 指定されたリソースに対してUAVバリアを設定します。
		/// </summary>
		/// <param name="commandList">バリアを記録するグラフィックスコマンドリスト。</param>
		/// <param name="resource">バリアを設定する対象のリソース。</param>
		void UAVBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource);

		/// <summary>
		/// リソースの状態遷移バリアを設定します。
		/// </summary>
		/// <param name="commandList">バリアを記録するコマンドリスト。</param>
		/// <param name="resource">状態を遷移するリソース。</param>
		/// <param name="before">遷移前のリソース状態。</param>
		/// <param name="after">遷移後のリソース状態。</param>
		void TransitionBarrier(
			ID3D12GraphicsCommandList* commandList,
			ID3D12Resource* resource,
			D3D12_RESOURCE_STATES before,
			D3D12_RESOURCE_STATES after);
	};

}


