#include "RasterizerState.h"

///-------------------------------------------/// 
/// ラスタライザステートの設定
///-------------------------------------------///
namespace {
	const std::unordered_map<PipelineType, D3D12_CULL_MODE> kCullModeTable_ = {
	{ PipelineType::ForGround2D,		D3D12_CULL_MODE_NONE },
	{ PipelineType::BackGround2D,		D3D12_CULL_MODE_NONE },
	{ PipelineType::Obj3D,				D3D12_CULL_MODE_BACK },
	{ PipelineType::PrimitiveSkyBox,    D3D12_CULL_MODE_NONE },
	{ PipelineType::Particle,			D3D12_CULL_MODE_NONE },
	{ PipelineType::Skinning3D,			D3D12_CULL_MODE_BACK },
	{ PipelineType::Line3D,				D3D12_CULL_MODE_NONE },
	{ PipelineType::OffScreen,			D3D12_CULL_MODE_NONE },
	{ PipelineType::Grayscale,			D3D12_CULL_MODE_NONE },
	{ PipelineType::Vignette,			D3D12_CULL_MODE_NONE },
	{ PipelineType::Dissolve,			D3D12_CULL_MODE_NONE },
	{ PipelineType::BoxFilter3x3,		D3D12_CULL_MODE_NONE },
	{ PipelineType::BoxFilter5x5,		D3D12_CULL_MODE_NONE },
	{ PipelineType::RadiusBlur,			D3D12_CULL_MODE_NONE },
	{ PipelineType::OutLine,			D3D12_CULL_MODE_NONE },
	{ PipelineType::ShatterGlass,		D3D12_CULL_MODE_NONE },
	};
}

///-------------------------------------------/// 
/// ラスタライザステートの生成
///-------------------------------------------///
void RasterizerState::Create(PipelineType Type) {
	// CullModeをテーブルから取得
	auto it = kCullModeTable_.find(Type);
	if (it != kCullModeTable_.end()) {
		rasterizerDesc_.CullMode = it->second;
	} else {
		rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE; // fallback
	}
	
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID; // 全て共通の設定
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
D3D12_RASTERIZER_DESC RasterizerState::GetRasterizerState()const { return rasterizerDesc_; }