#include "RadiusBlurEffect.h"
// Service
#include "Engine/System/Service/Render.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void RadiusBlurEffect::Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> RenderTexture) {
	// RenderTextureを取得
	renderTexture_ = RenderTexture;

	// bufferの作成
	buffer_ = std::make_unique<BufferBase>();
	buffer_->Create(device, sizeof(RadiusBlurData));
	buffer_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	// RadiusBlurエフェクトのデータを初期化7
	data_->center = { 0.5f, 0.5f }; // 中心座標を画面中央に設定
	data_->numSamples = 16; // デフォルトのサンプリング数
	data_->blurWidth = 0.01f; // デフォルトのブラー幅
}

///-------------------------------------------/// 
/// 描画前処理
///-------------------------------------------///
void RadiusBlurEffect::PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {
	RenderPass::PreDraw(commandList, dsvHandle);
}

///-------------------------------------------/// 
/// ImGuiの描画
///-------------------------------------------///
void RadiusBlurEffect::Draw(ID3D12GraphicsCommandList* commandList) {
	// コピーイメージ
	Render::SetPSO(commandList, PipelineType::RadiusBlur, BlendMode::kBlendModeNone);

	// dataの設定
	commandList->SetGraphicsRootConstantBufferView(1, buffer_->GetBuffer()->GetGPUVirtualAddress());

	commandList->SetGraphicsRootDescriptorTable(0, renderTexture_->GetSRVHandle());
	// 頂点3つを描画
	commandList->DrawInstanced(3, 1, 0, 0);
}

///-------------------------------------------/// 
/// ImGui情報
///-------------------------------------------///
void RadiusBlurEffect::ImGuiInfo() {
#ifdef USE_IMGUI
	// ImGuiの描画
	ImGui::Text("RadiusBlur Effect");
	ImGui::DragFloat2("center", &data_->center.x, 0.1f);
	ImGui::SliderInt("numSamples", &data_->numSamples, 1, 100);
	ImGui::SliderFloat("blurWidth", &data_->blurWidth, 0.0f, 1.0f);
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void RadiusBlurEffect::SetRadiusBlurData(const Vector2& center, int numSamples, float blurWidth) {
	data_->center = center;
	data_->numSamples = numSamples;
	data_->blurWidth = blurWidth;
}
