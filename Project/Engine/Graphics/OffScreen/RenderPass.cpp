#include "RenderPass.h"

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void RenderPass::PreDraw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandel = renderTexture_->GetRTVHandle();
	commandList->OMSetRenderTargets(1, &rtvHandel, false, &dsvHandle);

	// RenderTextureのクリア
	renderTexture_->Clear(commandList);
}