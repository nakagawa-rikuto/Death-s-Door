
#include "VignetteEffect.h"
// Service
#include "Engine/System/Service/Render.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif


///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void VignetteEffect::Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> RenderTexture) {
	// RenderTextureを取得
	renderTexture_ = RenderTexture;

	// bufferの作成
	buffer_ = std::make_unique<BufferBase>();
	buffer_->Create(device, sizeof(VignetteData));
	buffer_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	// Vignetteエフェクトのデータを初期化
	data_->scale = 16.0f; // デフォルトのスケール
	data_->pawer = 0.8f; // デフォルトのパワー
}

///-------------------------------------------/// 
/// 描画前処理
///-------------------------------------------///
void VignetteEffect::PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {
	RenderPass::PreDraw(commandList, dsvHandle);
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void VignetteEffect::Draw(ID3D12GraphicsCommandList* commandList) {
	// パイプラインの設定
	Render::SetPSO(commandList, PipelineType::Vignette, BlendMode::kBlendModeNone);

	// dataの設定
	commandList->SetGraphicsRootConstantBufferView(1, buffer_->GetBuffer()->GetGPUVirtualAddress());

	commandList->SetGraphicsRootDescriptorTable(0, renderTexture_->GetSRVHandle());
	// 頂点3つを描画
	commandList->DrawInstanced(3, 1, 0, 0);
}

///-------------------------------------------/// 
/// ImGui情報
///-------------------------------------------///
void VignetteEffect::ImGuiInfo() {
#ifdef USE_IMGUI
	// ImGuiの描画
	ImGui::Text("Vignette Effect");
	ImGui::SliderFloat("Scale", &data_->scale, 0.0f, 1.0f);
	ImGui::SliderFloat("Pawer", &data_->pawer, 0.0f, 1.0f);
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void VignetteEffect::SetData(VignetteData data) {
	data_->scale = data.scale;
	data_->pawer = data.pawer;
}
