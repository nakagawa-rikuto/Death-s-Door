#include "FFTOceanGenerator.h"
// CameraCommon
#include "Engine/Camera/Base/CameraCommon.h"
// Service
#include "Service/DeltaTime.h"
#include "Service/GraphicsResourceGetter.h"
#include "Service/Render.h"
// C++
#include <cassert>
#include <cmath>
// Math
#include "Math/MatrixMath.h"
#ifdef USE_IMGUI
#include <imgui.h>// ImGui
#endif // USE_IMGUI

namespace MiiEngine {
	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	FFTOceanGenerator::~FFTOceanGenerator() {
		// Unmap
		if (vertex_ && vertex_->GetBuffer()) {
			vertex_->GetBuffer()->Unmap(0, nullptr);
		}
		if (index_ && index_->GetBuffer()) {
			index_->GetBuffer()->Unmap(0, nullptr);
		}
		vertex_.reset();
		index_.reset();
		base_.reset();
		compute_.reset();
		ripplenInjections_.clear();
	}

	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	void FFTOceanGenerator::Initialize(int gridSize) {
		ID3D12Device* device = Service::GraphicsResourceGetter::GetDXDevice();
		assert(device);

		gridSize_ = gridSize;

		// Countの設定
		vertexCount_ = (gridSize_ + 1) * (gridSize_ + 1);
		indexCount_ = gridSize_ * gridSize_ * 6;

		/// ===FFTOceanBase=== ///
		// 初期化
		base_ = std::make_unique<FFTOceanBase>();
		base_->Initialize(device);

		/// ===FFTOceanCompute=== ///
		// 初期化
		compute_ = std::make_unique<FFTOceanCompute>();
		compute_->Initialize(device, gridSize_);

		// Compute側のSRVインデックスをBaseに渡す
		base_->SetDisplaceSRVIndex(compute_->GetSRVDisplaceIndex());
		base_->SetNormalFoamSRVIndex(compute_->GetSRVNormalFoamIndex());
		base_->SetRippleSRVIndex(compute_->GetRippleSimulator()->GetCurrentSRVIndex());
		// テクスチャの設定
		base_->SetFoamTextureName("oceanFFT");

		/// ===vertex=== ///
		CreateVertexBuffer(device);

		/// ===Index=== ///
		CreateIndexBuffer(device);

		// ワールド行列の初期化
		SetTileScale(1.0f);
	}

	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void FFTOceanGenerator::Update() {
		float deltaTime = Service::DeltaTime::GetDeltaTime();

		// ワールド行列を更新してBaseに渡す
		UpdateWorldMatrix();

		// Compute側の時間更新
		compute_->Update(deltaTime);

		// Base側の更新
		base_->Update();
	}

	///-------------------------------------------/// 
	/// 描画
	///-------------------------------------------///
	void FFTOceanGenerator::Draw(BlendMode mode) {
		// コマンドリストの取得
		ID3D12GraphicsCommandList* commandList = Service::GraphicsResourceGetter::GetDXCommandList();
		assert(commandList);

		// 波紋
		FlushRipplenInjections(commandList);

		// コンピュートシェーダーのディスパッチ
		compute_->Dispatch(commandList);

		// RippleのSRVインデックス（最新）をBaseに渡す
		base_->SetRippleSRVIndex(compute_->GetRippleSimulator()->GetCurrentSRVIndex());

		// パイプラインのセット
		Service::Render::SetPSO(commandList, PipelineType::FFTOcean, mode);

		// 頂点バッファ・インデックスバッファをセット
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
		commandList->IASetIndexBuffer(&indexBufferView_);
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// CBV・テクスチャのバインド
		base_->Bind(commandList);

		// 描画
		commandList->DrawIndexedInstanced(indexCount_, 1, 0, 0, 0);

		// 次フレームのためにリソース状態をリセット
		compute_->ResetResourceStatesForNextFrame(commandList);
	}

	///-------------------------------------------/// 
	/// ImGui表示
	///-------------------------------------------///
	void FFTOceanGenerator::ShowImGui() {
#ifdef _DEBUG
		if (ImGui::Begin("FFTOcean")) {

			// --- Transform ---
			if (ImGui::CollapsingHeader("Transform")) {
				ImGui::DragFloat3("Translate", &transform_.translate.x, 0.1f);
				float size = base_->GetSize();
				ImGui::DragFloat("Size", &size, 0.1f);
				SetSize(size);
			}

			// --- TransformCB（VS用）---
			if (ImGui::CollapsingHeader("TransformCB")) {
				float tileScale = base_->GetTileScale();
				ImGui::DragFloat("タイルスケール", &tileScale, 0.1f, 0.1f, 10.0f);
				base_->SetTileScale(tileScale);
			}

			// --- OceanRenderCB（PS用）---
			if (ImGui::CollapsingHeader("色の設定")) {
				OceanRenderCB& r = base_->GetOceanRenderCB();
				ImGui::DragFloat3("光の方向", &r.sunDirection.x, 0.01f, -1.0f, 1.0f);
				ImGui::DragFloat("光の強さ", &r.sunPower, 0.1f, 0.0f, 5.0f);
				ImGui::ColorEdit3("光の色", &r.sunColor.x);
				ImGui::DragFloat("フレネルバイアス", &r.fresnelBias, 0.001f, 0.02f, 0.2f);
				ImGui::ColorEdit3("深海の色", &r.deepColor.x);
				ImGui::DragFloat("粗さ", &r.roughness, 0.01f, 0.0f, 1.0f);
				ImGui::ColorEdit3("浅瀬の色", &r.shallowColor.x);
				ImGui::DragFloat("SSSの強さ", &r.sssStrength, 0.01f, 0.0f, 3.0f);
				ImGui::ColorEdit3("泡の色", &r.foamColor.x);
				ImGui::DragFloat("泡の柔らかさ", &r.foamSoftness, 0.01f, 0.01f, 1.0f);
				ImGui::ColorEdit3("空の色（地平線）", &r.skyColorHorizon.x);
				ImGui::ColorEdit3("空の色（天頂）", &r.skyColorZenith.x);
			}

			// --- OceanParams（CS用）---
			if (ImGui::CollapsingHeader("波のパラメータ")) {
				OceanParams& p = compute_->GetOceanParams();
				ImGui::DragFloat("グリッド幅", &p.gridWidth, 0.1f, 0.1f, 50000.0f);
				ImGui::DragFloat("風速", &p.windowSpeed, 0.1f, 0.0f, 100.0f);
				ImGui::DragFloat2("風向", &p.windDirection.x, 0.01f, -1.0f, 1.0f);
				ImGui::DragFloat("振幅", &p.amplitude, 0.0001f, 0.0001f, 50.0f);
				ImGui::DragFloat("波長", &p.lambda, 0.01f, 0.0f, 30.0f);
				ImGui::DragFloat("泡のしきい値", &p.foamThreshold, 0.01f, 0.0f, 5.0f);
			}

			/// ===Ripple（波紋）=== ///
			if (ImGui::CollapsingHeader("波紋の設定")) {
				RippleSimulator* ripple = compute_->GetRippleSimulator();

				float waveSpeed = ripple->GetWaveSpeed();
				if (ImGui::DragFloat("伝播速度", &waveSpeed, 0.1f, 0.5f, 100.0f)) {
					ripple->SetWaveSpeed(waveSpeed);
				}

				float damping = ripple->GetDamping();
				if (ImGui::DragFloat("減衰係数", &damping, 0.001f, 0.900f, 0.999f)) {
					ripple->SetDamping(damping);
				}

				if (ImGui::DragFloat2("出現位置UV", &ripplePos_.x, 0.1f)) {
					ripple->SetInjectionUV(ripplePos_);
				}

				// テスト用: ボタンでグリッド中央に波紋を発生
				ImGui::Separator();
				static float testRadius = 5.0f;
				static float testStrength = 1.0f;
				ImGui::DragFloat("テスト半径", &testRadius, 0.1f, 0.1f, 150.0f);
				ImGui::DragFloat("テスト強度", &testStrength, 0.1f, 0.1f, 10.0f);
				if (ImGui::Button("中央に波紋を発生")) {
					AddRipple({ripplePos_.x, 0.0f, ripplePos_.y}, testRadius, testStrength);
				}
			}
		}
		ImGui::End();
#endif // _DEBUG
	}


	///-------------------------------------------/// 
	/// 波紋の追加
	///-------------------------------------------///
	void FFTOceanGenerator::AddRipple(const Vector3& worldPos, float radius, float strength) {

		float posX = (worldPos.x - transform_.translate.x) / transform_.scale.x;
		float posZ = (worldPos.z - transform_.translate.z) / transform_.scale.z;

		/*posX = posX + 0.5f;*/
		posZ = posZ + 0.5f;

		float uvX = posZ;
		float uvY = posX;

		float rippleRadius = radius / transform_.scale.x;

		// 波紋を追加
		ripplenInjections_.push_back({ {uvX, uvY}, rippleRadius, strength });
	}

	///-------------------------------------------/// 
	/// Getter
	///-------------------------------------------///
	// OceanParamsの取得
	OceanParams& FFTOceanGenerator::GetOceanParams() const { return compute_->GetOceanParams(); }
	// OceanRenderCBの取得
	OceanRenderCB& FFTOceanGenerator::GetOceanRenderCB() const { return base_->GetOceanRenderCB(); }

	///-------------------------------------------/// 
	/// Setter
	///-------------------------------------------///
	// Camera
	void FFTOceanGenerator::SetCamera(CameraCommon* camera) { base_->SetCamera(camera); }
	// 座標の設定
	void FFTOceanGenerator::SetTranslate(const Vector3& translate) { transform_.translate = translate; }
	// 回転の設定
	void FFTOceanGenerator::SetRotate(const Quaternion& rotate) { transform_.rotate = rotate; }
	// スケールの設定
	void FFTOceanGenerator::SetScale(const Vector3& scale) { transform_.scale = scale; }
	// OceanRenderCBの設定
	void FFTOceanGenerator::SetOceanRenderCB(const OceanRenderCB& data) { base_->SetOceanRenderCB(data); }
	// OceanParamsの設定（CS側）
	void FFTOceanGenerator::SetOceanParams(const OceanParams& params) { compute_->SetOceanParams(params); }
	// タイルスケールの設定
	void FFTOceanGenerator::SetTileScale(float tileScale) { base_->SetTileScale(tileScale); }
	// サイズの設定
	void FFTOceanGenerator::SetSize(float size) { 
		base_->SetSize(size); 
		transform_.scale = { size, 1.0f, size }; // サイズに合わせてスケールも更新
	}

	///-------------------------------------------/// 
	/// 頂点バッファの生成
	///-------------------------------------------///
	void FFTOceanGenerator::CreateVertexBuffer(ID3D12Device* device) {
		vertex_ = std::make_unique<BufferBase>();
		vertex_->Create(device, sizeof(OceanGridVertex) * vertexCount_);

		// マッピング
		void* mapped = nullptr;
		vertex_->GetBuffer()->Map(0, nullptr, &mapped);
		vertexData_ = static_cast<OceanGridVertex*>(mapped);

		// グリッド頂点データを生成（XZ 平面上、Y=0）
		// UV は [0, 1] で正規化、後で VS 側で TileScale を掛ける
		for (int z = 0; z <= gridSize_; ++z) {
			for (int x = 0; x <= gridSize_; ++x) {
				int index = z * (gridSize_ + 1) + x;
				float fx = static_cast<float>(x) / static_cast<float>(gridSize_);
				float fz = static_cast<float>(z) / static_cast<float>(gridSize_);

				// Position: XZ 平面上に [-0.5, 0.5] の範囲で配置
				vertexData_[index].position = { fx - 0.5f, 0.0f, fz - 0.5f };
				// TexCoord: [0, 1] で正規化
				vertexData_[index].texCoord = { fx, fz };
			}
		}

		// VertexBufferView の設定
		vertexBufferView_.BufferLocation = vertex_->GetBuffer()->GetGPUVirtualAddress();
		vertexBufferView_.SizeInBytes = sizeof(OceanGridVertex) * vertexCount_;
		vertexBufferView_.StrideInBytes = sizeof(OceanGridVertex);
	}

	///-------------------------------------------/// 
	/// インデックスバッファの生成
	///-------------------------------------------///
	void FFTOceanGenerator::CreateIndexBuffer(ID3D12Device* device) {
		index_ = std::make_unique<BufferBase>();
		index_->Create(device, sizeof(uint32_t) * indexCount_);

		// マッピング
		void* mapped = nullptr;
		index_->GetBuffer()->Map(0, nullptr, &mapped);
		indexData_ = static_cast<uint32_t*>(mapped);

		// インデックスデータを生成
		uint32_t idx = 0;
		for (int z = 0; z < gridSize_; ++z) {
			for (int x = 0; x < gridSize_; ++x) {
				uint32_t topLeft = z * (gridSize_ + 1) + x;
				uint32_t topRight = z * (gridSize_ + 1) + x + 1;
				uint32_t bottomLeft = (z + 1) * (gridSize_ + 1) + x;
				uint32_t bottomRight = (z + 1) * (gridSize_ + 1) + x + 1;

				// 三角形1: topLeft → bottomLeft → topRight
				indexData_[idx++] = topLeft;
				indexData_[idx++] = bottomLeft;
				indexData_[idx++] = topRight;

				// 三角形2: topRight → bottomLeft → bottomRight
				indexData_[idx++] = topRight;
				indexData_[idx++] = bottomLeft;
				indexData_[idx++] = bottomRight;
			}
		}

		// IndexBufferView の設定
		indexBufferView_.BufferLocation = index_->GetBuffer()->GetGPUVirtualAddress();
		indexBufferView_.SizeInBytes = sizeof(uint32_t) * indexCount_;
		indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	}

	///-------------------------------------------/// 
	/// ワールド行列の更新
	///-------------------------------------------///
	void FFTOceanGenerator::UpdateWorldMatrix() {
		// Scale → Rotate → Translate のSRT行列を合成
		Matrix4x4 worldMatrix = Math::MakeAffineQuaternionMatrix(transform_.scale, transform_.rotate, transform_.translate);
		base_->SetWorldMatrix(worldMatrix);
	}

	///-------------------------------------------/// 
	/// 波紋をGPUへ 
	///-------------------------------------------///
	void FFTOceanGenerator::FlushRipplenInjections(ID3D12GraphicsCommandList* commandList) {
		if (ripplenInjections_.empty()) {
			return;
		}
		// 参照
		RippleSimulator* ripple = compute_->GetRippleSimulator();
		// 波紋の追加
		for (const RippleInjection& r : ripplenInjections_) {
			ripple->AddRipple(commandList, r.uv, r.radius, r.strength);
		}

		// 波紋をGPUに送った後はクリア
		ripplenInjections_.clear();
	}
}
