#include "OffScreenRenderer.h"
// Effect
#include "Effect/CopyImageEffect.h"
#include "Effect/GrayscaleEffect.h"
#include "Effect/VignetteEffect.h"
#include "Effect/OutLineEffect.h"
#include "Effect/BoxFilter3x3Effect.h"
#include "Effect/BoxFilter5x5Effect.h"
#include "Effect/RadiusBlurEffect.h"
#include "Effect/DissolveEffect.h"
#include "Effect/ShatterGlassEffect.h"
#include "Effect/BloomEffect.h"
// Service
#include "Service/Render.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif

namespace MiiEngine {
	///-------------------------------------------/// 
	/// テクスチャの状態を遷移させる関数
	///-------------------------------------------///
	namespace {
		void TransitionResource(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) {
			if (stateBefore == stateAfter) return;
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = resource;
			barrier.Transition.StateBefore = stateBefore;
			barrier.Transition.StateAfter = stateAfter;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			cmdList->ResourceBarrier(1, &barrier);
		}
	}

	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	OffScreenRenderer::~OffScreenRenderer() {
		activePass_ = nullptr;
		renderPass_.clear();
		sceneTexture_.reset();
		for (int i = 0; i < 2; ++i) {
			pingPongTextures_[i].reset();
		}
	}

	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	void OffScreenRenderer::Initialize(
		ID3D12Device* device,
		SRVManager* srv, RTVManager* rtv,
		uint32_t width, uint32_t height, const Vector4& clearColor) {

		// シーン描画用のRenderTextureを初期化
		sceneTexture_ = std::make_shared<RenderTexture>();
		sceneTexture_->Initialize(srv, rtv, width, height, clearColor, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		sceneTexture_->CreateRenderTexture(device);

		// エフェクト適用後のRenderTextureを初期化
		for (int i = 0; i < 2; ++i) {
			pingPongTextures_[i] = std::make_shared<RenderTexture>();
			pingPongTextures_[i]->Initialize(srv, rtv, width, height, clearColor, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
			pingPongTextures_[i]->CreateRenderTexture(device);
		}

		/// ===RenderPassの登録=== ///
		// OffScreenTypeに対応するRenderPassを作成して登録
		renderPass_[OffScreenType::CopyImage] = std::make_unique<CopyImageEffect>();
		renderPass_[OffScreenType::Grayscale] = std::make_unique<GrayscaleEffect>();
		renderPass_[OffScreenType::Vignette] = std::make_unique<VignetteEffect>();
		renderPass_[OffScreenType::BoxFilter3x3] = std::make_unique<BoxFilter3x3Effect>();
		renderPass_[OffScreenType::BoxFilter5x5] = std::make_unique<BoxFilter5x5Effect>();
		renderPass_[OffScreenType::RadiusBlur] = std::make_unique<RadiusBlurEffect>();
		renderPass_[OffScreenType::OutLine] = std::make_unique<OutLineEffect>();
		renderPass_[OffScreenType::Dissolve] = std::make_unique<DissolveEffect>();
		renderPass_[OffScreenType::ShatterGlass] = std::make_unique<ShatterGlassEffect>();
		renderPass_[OffScreenType::Bloom] = std::make_unique<BloomEffect>();

		// 各RenderPassの初期化と入力テクスチャの設定
		for (auto& [type, pass] : renderPass_) {
			pass->Initialize(device, pingPongTextures_[0]);
		}

		// デフォルトの設定
		ClearEffects();
		AddEffect(OffScreenType::CopyImage);
	}

	///-------------------------------------------/// 
	/// 描画前処理
	///-------------------------------------------///
	void OffScreenRenderer::PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {
		// シーン描画はsceneTexture_へ
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandel = sceneTexture_->GetRTVHandle();
		commandList->OMSetRenderTargets(1, &rtvHandel, false, &dsvHandle);
		// RenderTextureのクリア
		sceneTexture_->Clear(commandList);
	}

	///-------------------------------------------/// 
	/// 描画処理
	///-------------------------------------------///
	void OffScreenRenderer::Draw(ID3D12GraphicsCommandList* commandList) {
		// エフェクトが登録されていない場合は描画しない
		if (activeEffects_.empty()) return;

		bool isFirstPass = true;
		int readIndex = 0;  // 読み込み用テクスチャのインデックス
		int writeIndex = 1; // 書き込み用テクスチャのインデックス

		// ピンポンバッファの両方をクリアしておく
		pingPongTextures_[0]->Clear(commandList);
		pingPongTextures_[1]->Clear(commandList);

		for (auto type : activeEffects_) {
			auto* pass = renderPass_[type].get();

			// 最初のエフェクトだけsceneTexture_を入力にする
			std::shared_ptr<RenderTexture> currentInput = isFirstPass ? sceneTexture_ : pingPongTextures_[readIndex];
			std::shared_ptr<RenderTexture> currentOutput = pingPongTextures_[writeIndex];

			// リソースの状態を描画用に遷移
			if (!isFirstPass) {
				TransitionResource(commandList, currentInput->GetBuffer(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			}

			// RenderPassに入力テクスチャを設定
			pass->SetInputTexture(currentInput);

			// 描画先のRTVを設定
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = currentOutput->GetRTVHandle();
			commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

			// エフェクトの描画
			pass->Draw(commandList);

			// 描画後のリソース状態を次のエフェクト用に遷移
			if (!isFirstPass) {
				TransitionResource(commandList, currentInput->GetBuffer(),
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
			}

			// 次のエフェクトの為にPing-Pong
			std::swap(readIndex, writeIndex);
			isFirstPass = false;
		}

		// 結果が1なら0に入れるようにする
		if (readIndex == 1) {
			// [0] を コピー先(COPY_DEST) に、[1] を コピー元(COPY_SOURCE) に遷移
			TransitionResource(commandList, pingPongTextures_[0]->GetBuffer(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);

			TransitionResource(commandList, pingPongTextures_[1]->GetBuffer(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);

			// GPUの超高速コピー処理を実行！
			commandList->CopyResource(pingPongTextures_[0]->GetBuffer(), pingPongTextures_[1]->GetBuffer());

			// 次のフレームやSwapChainへの出力に備えて、両方を RENDER_TARGET に戻す
			TransitionResource(commandList, pingPongTextures_[0]->GetBuffer(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);

			TransitionResource(commandList, pingPongTextures_[1]->GetBuffer(),
				D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		}
	}

	///-------------------------------------------/// 
	/// 描画後処理
	///-------------------------------------------///
	void OffScreenRenderer::PostDraw(ID3D12GraphicsCommandList* commandList) {
		commandList;
	}

	///-------------------------------------------/// 
	/// effectTexture を SwapChain にコピー
	///-------------------------------------------///
	void OffScreenRenderer::CopyToSwapChain(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE swapChainRTV) {
		// SwapChain の RTV を描画先として設定
		commandList->OMSetRenderTargets(1, &swapChainRTV, false, nullptr);
		Service::Render::SetPSO(commandList, PipelineType::OffScreen, BlendMode::kBlendModeNone);

		// 最終的なテクスチャを取得
		auto finalTexture = GetFinalResultTexture();

		// 最終結果のテクスチャ(SRV)をセットして描画
		commandList->SetGraphicsRootDescriptorTable(0, finalTexture->GetSRVHandle());
		commandList->DrawInstanced(3, 1, 0, 0);
	}

	///-------------------------------------------/// 
	/// エフェクトリストをクリア
	///-------------------------------------------///
	void OffScreenRenderer::ClearEffects() {
		activeEffects_.clear();
	}

	///-------------------------------------------/// 
	/// エフェクトを追加
	///-------------------------------------------///
	void OffScreenRenderer::AddEffect(OffScreenType type) {
		// すでにリストに存在しない場合のみ追加
		if (std::find(activeEffects_.begin(), activeEffects_.end(), type) == activeEffects_.end()) {
			activeEffects_.push_back(type);
		}
	}

	///-------------------------------------------/// 
	/// 特定のエフェクトをリストから削除
	///-------------------------------------------///
	void OffScreenRenderer::RemoveEffect(OffScreenType type) {
		auto it = std::remove(activeEffects_.begin(), activeEffects_.end(), type);
		activeEffects_.erase(it, activeEffects_.end());

		// エフェクトが空になったらデフォルトのCopyImageエフェクトを追加
		if (activeEffects_.empty()) {
			AddEffect(OffScreenType::CopyImage);
		}
	}

#ifdef USE_IMGUI
	///-------------------------------------------/// 
	/// ImGuiの描画
	///-------------------------------------------///
	void OffScreenRenderer::DrawImGui() {
		// OffScreenType の名前と enum の対応リスト
		struct EffectInfo {
			const char* name;
			OffScreenType type;
		};

		const EffectInfo effectInfos[] = {
			{ "Grayscale",    OffScreenType::Grayscale },
			{ "Vignette",     OffScreenType::Vignette },
			{ "BoxFilter3x3", OffScreenType::BoxFilter3x3 },
			{ "BoxFilter5x5", OffScreenType::BoxFilter5x5 },
			{ "RadiusBlur",   OffScreenType::RadiusBlur },
			{ "OutLine",      OffScreenType::OutLine },
			{ "Dissolve",     OffScreenType::Dissolve },
			{ "ShatterGlass", OffScreenType::ShatterGlass },
			{ "Bloom",		  OffScreenType::Bloom }
		};

		if (ImGui::Begin("OffScreen Effect")) {
			if (ImGui::Button("Clear All Effects")) {
				ClearEffects();
			}
			ImGui::Separator();

			for (const auto& info : effectInfos) {
				bool isActive = std::find(activeEffects_.begin(), activeEffects_.end(), info.type) != activeEffects_.end();

				if (ImGui::Checkbox(info.name, &isActive)) {
					if (isActive) AddEffect(info.type);
					else RemoveEffect(info.type);
				}
			}

			ImGui::Separator();
			ImGui::Text("Active Effects Order:");

			int index = 1;
			for (auto type : activeEffects_) {
				if (type == OffScreenType::CopyImage) continue;

				const char* effectName = "Unknown";
				for (const auto& info : effectInfos) {
					if (info.type == type) {
						effectName = info.name;
						break;
					}
				}

				if (ImGui::CollapsingHeader(effectName, ImGuiTreeNodeFlags_DefaultOpen)) {
					ImGui::PushID(index);
					renderPass_[type]->ImGuiInfo();
					ImGui::PopID();
				}
				index++;
			}
		}
		ImGui::End();
	}
#endif // USE_IMGUI

	///-------------------------------------------/// 
	/// Getter
	///-------------------------------------------///
	// RTV
	D3D12_CPU_DESCRIPTOR_HANDLE OffScreenRenderer::GetResultRTV() const { return GetFinalResultTexture()->GetRTVHandle(); }
	// SRV
	D3D12_GPU_DESCRIPTOR_HANDLE OffScreenRenderer::GetResultSRV() const { return GetFinalResultTexture()->GetSRVHandle(); }
	// RTVIndex
	uint32_t OffScreenRenderer::GetRTVHandleIndex() const { return GetFinalResultTexture()->GetRTVHandleIndex(); }
	// SRVIndex
	uint32_t OffScreenRenderer::GetSRVHandleIndex() const { return GetFinalResultTexture()->GetSRVHandleIndex(); }
	// Resource
	ID3D12Resource* OffScreenRenderer::GetSceneBuffer() const { return sceneTexture_->GetBuffer(); }
	ID3D12Resource* OffScreenRenderer::GetEffectBuffer() const { return GetFinalResultTexture()->GetBuffer(); }

	/// ===Effect=== ///
	RenderPass* OffScreenRenderer::GetRenderPass(OffScreenType type) const { return renderPass_.at(type).get(); }

	/// ===Type=== ///
	OffScreenType OffScreenRenderer::GetType() {
		if (activeEffects_.empty()) return OffScreenType::CopyImage; // デフォルトのエフェクト
		return activeEffects_.back(); // 最後に追加されたエフェクトが現在のタイプ
	}

	///-------------------------------------------/// 
	/// Setter
	///-------------------------------------------///
	// Typeの設定
	void OffScreenRenderer::SetType(OffScreenType type) {
		// タイプが変更された場合のみ処理
		ClearEffects();

		// タイプを設定
		AddEffect(type);
	}

	///-------------------------------------------/// 
	/// 最終結果のテクスチ
	///-------------------------------------------///
	std::shared_ptr<RenderTexture> OffScreenRenderer::GetFinalResultTexture() const {
		return pingPongTextures_[0]; // 常に0番を返す
	}
}