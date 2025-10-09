#include "PipelineStateObjectCommon.h"
// Compiler
#include "Engine/Graphics/Pipeline/Compiler.h"
// Engine
#include "Engine/Core/DXCommon.h"
// c++
#include <cassert>

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
PipelineStateObjectCommon::PipelineStateObjectCommon() = default;

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
PipelineStateObjectCommon::~PipelineStateObjectCommon() {

	rootSignature_.reset();
	inputLayout_.reset();
	blendState_.reset();
	rasterizerState_.reset();
	depthStencil_.reset();
	graphicsPipelineState_.Reset();
}

///-------------------------------------------/// 
/// PSOの作成
///-------------------------------------------///
void PipelineStateObjectCommon::Create(DXCommon* dxCommon, Compiler* compiler, PipelineType Type, BlendMode Mode) {

	// RootSignatureの生成
	rootSignature_ = std::make_unique<RootSignature>();
	rootSignature_->Create(dxCommon, Type);

	// InputLayoutの生成
	inputLayout_ = std::make_unique<InputLayout>();
	inputLayout_->Create(Type);

	// BlendStateの生成
	blendState_ = std::make_unique<BlendState>();
	blendState_->Create(Mode);

	// RasterizerStateの生成
	rasterizerState_ = std::make_unique<RasterizerState>();
	rasterizerState_->Create(Type);

	// DepthStencilの生成
	depthStencil_ = std::make_unique<DepthStencil>();
	depthStencil_->Create(Type);

	// Compilerの初期化
	compiler_ = compiler;

	// PipelineState
	CreatePipelineState(dxCommon, Type);
}

///-------------------------------------------/// 
/// パイプラインの設定
///-------------------------------------------///
void PipelineStateObjectCommon::SetPSO(ID3D12GraphicsCommandList* commandList) {

	// assertチェック
	assert(rootSignature_);
	assert(graphicsPipelineState_);

	// RootSignatureの設定
	commandList->SetGraphicsRootSignature(rootSignature_->GetRootSignature());
	// PSOの設定
	commandList->SetPipelineState(graphicsPipelineState_.Get());
}


///-------------------------------------------/// 
/// パイプラインの作成
///-------------------------------------------///
void PipelineStateObjectCommon::CreatePipelineState(DXCommon* dxCommon, PipelineType type) {
	HRESULT hr;

	// PSOの取得
	graphicsPipelineStateDesc_.pRootSignature = rootSignature_->GetRootSignature(); // RootSignature
	graphicsPipelineStateDesc_.InputLayout = inputLayout_->GetInputLayout(); // InputLayout
	graphicsPipelineStateDesc_.VS = {
		compiler_->GetObjVS()->GetBufferPointer(), compiler_->GetObjVS()->GetBufferSize() }; // VertexShader
	graphicsPipelineStateDesc_.PS = {
		compiler_->GetObjPS()->GetBufferPointer(), compiler_->GetObjPS()->GetBufferSize() }; // PixlShader
	graphicsPipelineStateDesc_.BlendState = blendState_->GetBlendDesc(); // BlendState
	graphicsPipelineStateDesc_.RasterizerState = rasterizerState_->GetRasterizerState(); // RasterizerState
	graphicsPipelineStateDesc_.DepthStencilState = depthStencil_->GetDepthStencilDesc(); // DepthStencil

	// 書き込むRTVの情報
	graphicsPipelineStateDesc_.NumRenderTargets = 1;
	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 書き込むDSVの情報
	graphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 利用するトポロジ(形状)のタイプ。三角形
	if (type == PipelineType::Line3D) {
		graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	} else {
		graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	}
	

	// どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc_.SampleDesc.Count = 1;
	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 実際に生成
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}