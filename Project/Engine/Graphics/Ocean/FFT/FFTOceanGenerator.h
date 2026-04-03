#pragma once
/// ===include=== ///
#include "Engine/Graphics/Ocean/FFT/FFTOceanBase.h"
#include "Engine/Graphics/Ocean/FFT/FFTOceanCompute.h"
#include "Engine/DataInfo/OceanData.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
// c++
#include <memory>
#include <vector>
#include <d3d12.h>

namespace MiiEngine {
	/// ===前方宣言=== ///
	class CameraCommon;

	///=====================================================/// 
	/// FFTOceanGenerator
	///=====================================================///
	class FFTOceanGenerator {
	public:

		FFTOceanGenerator() = default;
		~FFTOceanGenerator();

		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize(int gridSize);

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();

		/// <summary>
		/// 描画処理
		/// </summary>
		/// <param name="commandList">使用するコマンドリスト。</param>
		void Draw(BlendMode mode = BlendMode::KBlendModeNormal);

		/// <summary>
		/// ImGui表示処理
		/// </summary>
		void ShowImGui();

		/// <summary>
		/// 指定ワールド座標に波紋を発生させる。
		/// </summary>
		/// <param name="worldPos">波紋の中心ワールド座標（XZ 平面）</param>
		/// <param name="radius">波紋の初期半径（グリッドセル単位、推奨: 3〜10）</param>
		/// <param name="strength">波紋の強度（推奨: 0.5〜2.0）</param>
		void AddRipple(const Vector3& worldPos, float radius = 5.0f, float strength = 1.0f);

	public: /// ===Getter=== ///

		// OceanParamsの取得
		OceanParams& GetOceanParams() const;
		// OceanRenderCBの取得
		OceanRenderCB& GetOceanRenderCB() const;

	public: /// ===Setter=== ///

		// カメラの設定
		void SetCamera(CameraCommon* camera);
		// 座標の設定
		void SetTranslate(const Vector3& translate);
		// 回転の設定
		void SetRotate(const Quaternion& rotate);
		// スケールの設定
		void SetScale(const Vector3& scale);
		// OceanRenderCBの設定
		void SetOceanRenderCB(const OceanRenderCB& data);
		// OceanParamsの設定（CS側）
		void SetOceanParams(const OceanParams& params);
		// タイルスケールの設定
		void SetTileScale(float tileScale);
		// サイズの設定
		void SetSize(float size);

	private:

		// グリッドサイズ
		int gridSize_ = 256;

		/// ===バッファリソース=== ///
		std::unique_ptr<BufferBase> vertex_; // 頂点バッファ
		std::unique_ptr<BufferBase> index_;  // インデックスバッファ

		/// ===データポインタ=== ///
		OceanGridVertex* vertexData_ = nullptr;
		uint32_t* indexData_ = nullptr;

		/// ===バッファービュー=== ///
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
		D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

		// Count
		uint32_t vertexCount_ = 0;
		uint32_t indexCount_ = 0;

		/// ===コンポーネント=== ///
		std::unique_ptr<FFTOceanBase> base_;       // VS/PS用バッファ管理
		std::unique_ptr<FFTOceanCompute> compute_; // CS用バッファ・ディスパッチ管理

		/// ===Transform=== ///
		QuaternionTransform transform_;

		/// ===波紋=== ///
		std::vector<RippleInjection> ripplenInjections_;
		Vector2 ripplePos_;

	private:

		/// <summary>
		/// 頂点バッファの生成（XZ 平面グリッド）
		/// </summary>
		void CreateVertexBuffer(ID3D12Device* device);

		/// <summary>
		/// インデックスバッファの生成
		/// </summary>
		void CreateIndexBuffer(ID3D12Device* device);

		/// <summary>
		/// ワールド行列の更新
		/// </summary>
		void UpdateWorldMatrix();

		/// <summary>
		/// Ripplenインジェクションをフラッシュします。
		/// </summary>
		/// <param name="commandList">グラフィックスコマンドを記録するためのD3D12コマンドリスト。</param>
		void FlushRipplenInjections(ID3D12GraphicsCommandList* commandList);
	};
}