#pragma once
/// ===Include=== ///
// Engine
#include "Engine/Graphics/Pipeline/RootSignature.h"
#include "Engine/Graphics/Pipeline/InputLayout.h"
#include "Engine/Graphics/Pipeline/BlendState.h"
#include "Engine/Graphics/Pipeline/DepthStencil.h"
#include "Engine/Graphics/Pipeline/RasterizerState.h"
// PipelineType
#include "Engine/DataInfo/PipelineStateObjectType.h"
// c++
#include <memory>
#include <array>
#include <cassert>
// directX
#include <d3d12.h>
#include <dxgidebug.h>
#include <dxgi1_6.h>
#include <dxcapi.h>


/// ===前方宣言=== ///
class DXCommon;
class Compiler;

class PipelineStateObjectCommon {
public:

	PipelineStateObjectCommon();
	~PipelineStateObjectCommon();

	// PSOの生成
	void Create(DXCommon* dxcommon, Compiler* compiler, PipelineType Type, BlendMode Mode);
	// PSOのセット
	void SetPSO(ID3D12GraphicsCommandList* commandList);

private:/// ===変数=== ///

	/// ===PSOに必要な情報=== ///
	std::unique_ptr<RootSignature> rootSignature_;     // RootSignature
	std::unique_ptr<InputLayout> inputLayout_;         // InputLayout
	std::unique_ptr<BlendState> blendState_;           // BlendState
	std::unique_ptr<RasterizerState> rasterizerState_; // RasterizerState
	std::unique_ptr<DepthStencil> depthStencil_;       // DepthStencil
	Compiler* compiler_;                               // Compiler

	ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_{};

private:/// ===関数=== ///

	// PipelineStateの生成
	void CreatePipelineState(DXCommon* dxCommon, PipelineType type);
};