#pragma once
/// ===Include=== ///
// PipelineType
#include "Engine/DataInfo/PipelineStateObjectType.h"
// directX
#include <d3d12.h>
#include <unordered_map>

///=====================================================/// 
/// DepthStencil
///=====================================================///
class DepthStencil {
public:

	DepthStencil() = default;
	~DepthStencil() = default;

	/// <summary>
	/// デプスステンシルの生成
	/// </summary>
	void Create(PipelineType Type);

	/// <summary>
	/// デプスステンシルの取得
	/// </summary>
	/// <returns></returns>
	D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc() const;

private:

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{}; // DepthStencil
};