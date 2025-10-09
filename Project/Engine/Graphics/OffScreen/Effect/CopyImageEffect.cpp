
#include "CopyImageEffect.h"
// Service
#include "Engine/System/Service/Render.h"

///-------------------------------------------/// 
/// 初期化　
///-------------------------------------------///
void CopyImageEffect::Initialize(ID3D12Device* device, std::shared_ptr<RenderTexture> RenderTexture) {
	// RenderTextureを取得
	renderTexture_ = RenderTexture;

	// Deviceの初期化
	ID3D12Device* devicePtr = device;
}

///-------------------------------------------/// 
/// 描画前処理
///-------------------------------------------///
void CopyImageEffect::PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {
	RenderPass::PreDraw(commandList, dsvHandle);
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void CopyImageEffect::Draw(ID3D12GraphicsCommandList* commandList) {
	// コピーイメージ
	Render::SetPSO(commandList, PipelineType::OffScreen, BlendMode::kBlendModeNone);

	commandList->SetGraphicsRootDescriptorTable(0, renderTexture_->GetSRVHandle());
	// 頂点3つを描画
	commandList->DrawInstanced(3, 1, 0, 0);
}


///-------------------------------------------/// 
/// ImGui情報
///-------------------------------------------///
void CopyImageEffect::ImGuiInfo() {

}