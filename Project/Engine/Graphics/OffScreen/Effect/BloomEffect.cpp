#include "BloomEffect.h"
#include "Service/Render.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif

namespace MiiEngine {

	// リソース状態遷移用のヘルパー
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
	/// 初期化
	///-------------------------------------------///
	void BloomEffect::Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> outputTexture) {
		outputTexture_ = outputTexture;

		// 定数バッファの作成
		buffer_ = std::make_unique<BufferBase>();
		buffer_->Create(device, sizeof(BloomData));
		buffer_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&data_));

		// 初期値の設定
		data_->threshold = 0.8f;
		data_->brightness = { 0.2125f, 0.7154f, 0.0721f };
		data_->intensity = 1.0f;

		// ★ outputTexture から SRV/RTVマネージャーとサイズを取得して、内部テクスチャを作る
		uint32_t width = outputTexture->GetWidth();   
		uint32_t height = outputTexture->GetHeight(); 
		SRVManager* srv = outputTexture->GetSRVManager(); 
		RTVManager* rtv = outputTexture->GetRTVManager(); 
		// 輝度抽出用のテクスチャ
		luminanceTexture_ = std::make_shared<RenderTexture>();
		luminanceTexture_->Initialize(srv, rtv, width, height, { 0, 0, 0, 1 }, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		luminanceTexture_->CreateRenderTexture(device);

		// ブラー用のテクスチャ
		blurTexture_ = std::make_shared<RenderTexture>();
		blurTexture_->Initialize(srv, rtv, width, height, { 0, 0, 0, 1 }, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		blurTexture_->CreateRenderTexture(device);
	}

	void BloomEffect::PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {
		RenderPass::PreDraw(commandList, dsvHandle);
	}

	///-------------------------------------------/// 
	/// 描画 (3パス構成)
	///-------------------------------------------///
	void BloomEffect::Draw(ID3D12GraphicsCommandList* commandList) {

		// ==========================================
		// 輝度抽出パス (HighLuminance)
		// ==========================================
		D3D12_CPU_DESCRIPTOR_HANDLE rtvLumi = luminanceTexture_->GetRTVHandle();
		commandList->OMSetRenderTargets(1, &rtvLumi, false, nullptr);
		luminanceTexture_->Clear(commandList);

		Service::Render::SetPSO(commandList, PipelineType::HighLuminance, BlendMode::kBlendModeNone);
		commandList->SetGraphicsRootDescriptorTable(0, inputTexture_->GetSRVHandle()); // t0: 元画像
		commandList->SetGraphicsRootConstantBufferView(1, buffer_->GetBuffer()->GetGPUVirtualAddress()); // b0: BloomData
		commandList->DrawInstanced(3, 1, 0, 0);

		// 次のブラーのためにSRVへ遷移
		TransitionResource(commandList, luminanceTexture_->GetBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		// ==========================================
		// ブラーパス (BoxFilter5x5)
		// ==========================================
		D3D12_CPU_DESCRIPTOR_HANDLE rtvBlur = blurTexture_->GetRTVHandle();
		commandList->OMSetRenderTargets(1, &rtvBlur, false, nullptr);
		blurTexture_->Clear(commandList);

		Service::Render::SetPSO(commandList, PipelineType::BoxFilter5x5, BlendMode::kBlendModeNone);
		commandList->SetGraphicsRootDescriptorTable(0, luminanceTexture_->GetSRVHandle()); // t0: 輝度画像
		commandList->DrawInstanced(3, 1, 0, 0);

		// 次の合成のためにSRVへ遷移
		TransitionResource(commandList, blurTexture_->GetBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		// ==========================================
		// 合成パス (BloomComposite)
		// ==========================================
		D3D12_CPU_DESCRIPTOR_HANDLE rtvOut = outputTexture_->GetRTVHandle();
		commandList->OMSetRenderTargets(1, &rtvOut, false, nullptr);

		Service::Render::SetPSO(commandList, PipelineType::BloomComposite, BlendMode::kBlendModeNone); // 光なので加算合成がおすすめ

		commandList->SetGraphicsRootDescriptorTable(0, inputTexture_->GetSRVHandle()); // t0: 元画像
		commandList->SetGraphicsRootConstantBufferView(1, buffer_->GetBuffer()->GetGPUVirtualAddress()); // b0: BloomData
		commandList->SetGraphicsRootDescriptorTable(2, blurTexture_->GetSRVHandle());  // t1: ブラー画像

		commandList->DrawInstanced(3, 1, 0, 0);

		// 次フレームのClearに備えてRTVに戻す
		TransitionResource(commandList, luminanceTexture_->GetBuffer(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		TransitionResource(commandList, blurTexture_->GetBuffer(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	///-------------------------------------------/// 
	/// ImGui情報
	///-------------------------------------------///
	void BloomEffect::ImGuiInfo() {
#ifdef USE_IMGUI
		ImGui::Text("Bloom Effect");
		ImGui::SliderFloat("Threshold", &data_->threshold, 0.0f, 2.0f);
		ImGui::SliderFloat("Intensity", &data_->intensity, 0.0f, 5.0f);
		ImGui::ColorEdit3("Brightness Weight", &data_->brightness.x);
#endif
	}

	void BloomEffect::SetData(BloomData data) {
		*data_ = data;
	}
}