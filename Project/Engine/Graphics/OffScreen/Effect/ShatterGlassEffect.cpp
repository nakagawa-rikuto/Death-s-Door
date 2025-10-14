#include "ShatterGlassEffect.h"
// Service
#include "Engine/System/Service/Render.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif

///-------------------------------------------/// 
/// 初期化　
///-------------------------------------------///
void ShatterGlassEffect::Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> RenderTexture) {
	// RenderTextureを取得
	renderTexture_ = RenderTexture;

	// Bufferの作成
	buffer_ = std::make_unique<BufferBase>();
	buffer_->Create(device, sizeof(ShatterGlassData));
	buffer_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	// デフォルトパラメータの設定
	data_->progress = 0.0f;
	data_->impactX = 0.5f;
	data_->impactY = 0.5f;
	data_->crackDensity = 15.0f;
	data_->dispersion = 1.0f;
	data_->rotation = 1.0f;
	data_->fadeOut = 0.0f;
}

///-------------------------------------------/// 
/// 描画前処理
///-------------------------------------------///
void ShatterGlassEffect::PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {
	RenderPass::PreDraw(commandList, dsvHandle);
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void ShatterGlassEffect::Draw(ID3D12GraphicsCommandList* commandList) {

	// パイプラインの設定
	Render::SetPSO(commandList, PipelineType::ShatterGlass, BlendMode::kBlendModeNone);

	commandList->SetGraphicsRootConstantBufferView(1, buffer_->GetBuffer()->GetGPUVirtualAddress());

	// テクスチャとパラメータをセット
	commandList->SetGraphicsRootDescriptorTable(0, renderTexture_->GetSRVHandle());
	

	// 頂点3つを描画（フルスクリーン三角形）
	commandList->DrawInstanced(3, 1, 0, 0);
}

///-------------------------------------------/// 
/// ImGui情報
///-------------------------------------------///
void ShatterGlassEffect::ImGuiInfo() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("Shatter Glass Parameters")) {
		ImGui::SliderFloat("Progress", &data_->progress, 0.0f, 1.0f);
		ImGui::SliderFloat("Impact X", &data_->impactX, 0.0f, 1.0f);
		ImGui::SliderFloat("Impact Y", &data_->impactY, 0.0f, 1.0f);
		ImGui::SliderFloat("Crack Density", &data_->crackDensity, 5.0f, 50.0f);
		ImGui::SliderFloat("Dispersion", &data_->dispersion, 0.0f, 3.0f);
		ImGui::SliderFloat("Rotation", &data_->rotation, 0.0f, 5.0f);
		ImGui::SliderFloat("Fade Out", &data_->fadeOut, 0.0f, 1.0f);

		if (ImGui::Button("Reset")) {
			data_->progress = 0.0f;
			data_->impactX = 0.5f;
			data_->impactY = 0.5f;
			data_->crackDensity = 15.0f;
			data_->dispersion = 1.0f;
			data_->rotation = 1.0f;
			data_->fadeOut = 0.0f;
		}

		ImGui::TreePop();
	}
#endif
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void ShatterGlassEffect::SetData(ShatterGlassData data) {
	data_->progress = data.progress;
	data_->impactX = data.impactX;
	data_->impactY = data.impactY;
	data_->crackDensity = data.crackDensity;
	data_->dispersion = data.dispersion;
	data_->rotation = data.rotation;
	data_->fadeOut = data.fadeOut;
}
