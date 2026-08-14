#pragma once
/// ===include=== ///
#include "Engine/Graphics/Base/BufferBase.h"
#include "Engine/DataInfo/OceanData.h"
#include "Math/Matrix4x4.h"
// c++
#include <memory>
#include <d3d12.h>

namespace MiiEngine {
	/// ===前方宣言=== ///
	class CameraCommon;
	class SRVManager;

	///=====================================================/// 
	/// FFTOceanBase
	///=====================================================///
	class FFTOceanBase {
	public:

		FFTOceanBase() = default;
		~FFTOceanBase();

		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="device">初期化に使用するD3D12デバイスへのポインタ。</param>
		void Initialize(ID3D12Device* device);

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();

		/// <summary>
		/// 指定されたコマンドリストにバインドします。
		/// </summary>
		/// <param name="commandList">バインド先のDirect3D 12グラフィックスコマンドリスト。</param>
		void Bind(ID3D12GraphicsCommandList* commandList);

	public: /// ===Getter=== ///
		// OceanRenderCBの取得
		OceanRenderCB& GetOceanRenderCB();
		float GetTileScale() const;
		float GetSize() const;

	public: /// ===Setter=== ///
		// カメラの設定
		void SetCamera(CameraCommon* camera);
		// OceanRenderCBの設定
		void SetOceanRenderCB(const OceanRenderCB& data);
		// ワールド行列の設定
		void SetWorldMatrix(const Matrix4x4& world);
		// タイルスケールの設定
		void SetTileScale(float tileScale);
		// サイズの設定
		void SetSize(float size);
		// CS側のSRVインデックスを受け取る
		void SetDisplaceSRVIndex(uint32_t index);
		// CS側のSRVインデックスを受け取る
		void SetNormalFoamSRVIndex(uint32_t index);
		// CS側のSRVインデックスを受け取る
		void SetRippleSRVIndex(uint32_t index);
		// 泡テクスチャ名の設定
		void SetFoamTextureName(const std::string& name);

	protected:
		// SRVマネージャーへのポインタ
		SRVManager* srvManager_ = nullptr;

		/// ===バッファリソース=== ///
		// TransformCB(b0)VS
		std::unique_ptr<BufferBase> transformBuffer_;
		TransformCB* transformCBData_ = nullptr;
		// OceanRenderCB(b1)PS
		std::unique_ptr<BufferBase> oceanBuffer_;
		OceanRenderCB* oceanRenderCBData_ = nullptr;
		// テクスチャ３つ(t0~t1)
		uint32_t srvDisplaceIndex_ = 0;	  // t0
		uint32_t srvNormalFoamIndex_ = 0; // t1
		uint32_t srvRippleIndex_ = 0;      // t2
		std::string foamTextureName_;	  // t3

		/// ===カメラ=== ///
		CameraCommon* camera_ = nullptr;

		/// ===Transform=== ///
		Matrix4x4 worldMatrix_{};
		float tileScale_ = 1.0f;
		float size_ = 1.0f;

		/// ===OceanRenderCBデフォルト値=== ///
		OceanRenderCB oceanRenderCB_ = {
			{ 0.0f, -1.0f, 0.0f },    // sunDirection
			30.0f,                    // sunPower
			{ 1.0f, 0.95f, 0.8f },    // sunColor
			0.02f,                    // fresnelBias
			{ 0.0f, 0.05f, 0.1f },    // deepColor
			0.1f,                     // roughness
			{ 0.0f, 0.5f, 0.5f },     // shallowColor
			0.5f,                     // sssStrength
			{ 1.0f, 1.0f, 1.0f },     // foamColor
			0.5f,                     // foamSoftness
			{ 0.6f, 0.7f, 0.9f },     // skyColorHorizon
			0.0f,                     // pad0
			{ 0.1f, 0.2f, 0.5f },     // skyColorZenith
			1.0f                      // OceanOpacity
		};

	private:

		/// <summary>
		/// OceanRenderCBの書き込み処理
		/// </summary>
		void WriteOceanRenderCB();

		/// <summary>
		/// TransformCBの書き込み処理
		/// </summary>
		void WriteTransformCB();
	};
}


