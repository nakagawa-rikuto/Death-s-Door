#include "DepthStencil.h"

namespace {
	D3D12_DEPTH_STENCIL_DESC CreateDepthDesc(bool enable, D3D12_DEPTH_WRITE_MASK writeMask = D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC func = D3D12_COMPARISON_FUNC_ALWAYS, bool stencilEnable = false) {
		D3D12_DEPTH_STENCIL_DESC desc{};
		desc.DepthEnable = enable;
		desc.DepthWriteMask = writeMask;
		desc.DepthFunc = func;
		desc.StencilEnable = stencilEnable;
		return desc;
	}

	/// ===テーブルに登録=== ///
	const std::unordered_map<PipelineType, D3D12_DEPTH_STENCIL_DESC> kDepthStencilTable_ = {
		// 2D: 前景（深度有効, 書き込みあり, 比較常にOK）
		{ PipelineType::ForGround2D,  CreateDepthDesc(true,  D3D12_DEPTH_WRITE_MASK_ALL,    D3D12_COMPARISON_FUNC_ALWAYS) },

		// 2D: 背景（深度無効, 書き込みなし, 比較LessEqual, ステンシル無効）
		{ PipelineType::BackGround2D, CreateDepthDesc(false, D3D12_DEPTH_WRITE_MASK_ZERO,   D3D12_COMPARISON_FUNC_LESS_EQUAL, false) },

		// 3D Object（深度有効, 書き込みあり, 比較LessEqual）
		{ PipelineType::Obj3D,        CreateDepthDesc(true,  D3D12_DEPTH_WRITE_MASK_ALL,    D3D12_COMPARISON_FUNC_LESS_EQUAL) },

		// SkyBox（深度有効, 書き込みなし, 比較LessEqual）
		{ PipelineType::PrimitiveSkyBox,       CreateDepthDesc(true,  D3D12_DEPTH_WRITE_MASK_ZERO,   D3D12_COMPARISON_FUNC_LESS_EQUAL) },

		// Particle（深度有効, 書き込みなし, 比較LessEqual）
		{ PipelineType::Particle,     CreateDepthDesc(true,  D3D12_DEPTH_WRITE_MASK_ZERO,   D3D12_COMPARISON_FUNC_LESS_EQUAL) },

		// Skinning 3D（深度有効, 書き込みあり, 比較LessEqual）
		{ PipelineType::Skinning3D,   CreateDepthDesc(true,  D3D12_DEPTH_WRITE_MASK_ALL,    D3D12_COMPARISON_FUNC_LESS_EQUAL) },

		// Line3D （深度有効, 書き込みなし, 比較LessEqual）
		{ PipelineType::Line3D, CreateDepthDesc(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_LESS_EQUAL) },

		// PostEffect 系（深度無効）
		{ PipelineType::OffScreen,    CreateDepthDesc(false) },
		{ PipelineType::Grayscale,    CreateDepthDesc(false) },
		{ PipelineType::Vignette,     CreateDepthDesc(false) },
		{ PipelineType::Dissolve,     CreateDepthDesc(false) },
		{ PipelineType::BoxFilter3x3, CreateDepthDesc(false) },
		{ PipelineType::BoxFilter5x5, CreateDepthDesc(false) },
		{ PipelineType::RadiusBlur,   CreateDepthDesc(false) },
		{ PipelineType::OutLine,	  CreateDepthDesc(false) },
		{ PipelineType::ShatterGlass, CreateDepthDesc(false) },
	};
}



///-------------------------------------------/// 
/// デプスステンシルの生成
///-------------------------------------------///
void DepthStencil::Create(PipelineType Type) {
	auto it = kDepthStencilTable_.find(Type);
	if (it != kDepthStencilTable_.end()) {
		depthStencilDesc_ = it->second;
	} else {
		depthStencilDesc_ = {}; // fallback to default
	}
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
D3D12_DEPTH_STENCIL_DESC DepthStencil::GetDepthStencilDesc() const { return depthStencilDesc_; }