#include "BlendState.h"

///-------------------------------------------/// 
/// ブレンドステートの生成
///-------------------------------------------///
void BlendState::Create(BlendMode mode) {
	// BlendStateの設定
	// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	if (mode == BlendMode::kBlendModeAdd) {
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;

		// 加算合成
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		// a値の設定
		blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	} else if (mode == BlendMode::kBlendModeSubtract) {
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;

		// 減算合成
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		// a値の設定
		blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	} else if (mode == BlendMode::kBlendModeMultily) {
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;

		// 乗算合成
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;

		// a値の設定
		blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	} else if (mode == BlendMode::kBlendModeScreen) {
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;

		// スクリーン合成
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		// a値の設定
		blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	} else if (mode == BlendMode::KBlendModeNormal) {
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;

		// Normal
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		// a値の設定
		blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	}
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
D3D12_BLEND_DESC BlendState::GetBlendDesc() { return blendDesc_; }