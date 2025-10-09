#include "PiplineManager.h"

#include <cassert>

///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
PipelineManager::PipelineManager() = default;
PipelineManager::~PipelineManager() { 
	pipelines_.clear(); 
	compiler_.clear();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void PipelineManager::Initialize(DXCommon* dxCommon) {
	for (PipelineType type : AllPipelineTypes()) {

		// もしかしたらブレンドモード以外も作るべきかも
		auto compiler = std::make_unique<Compiler>();
		compiler->Initialize(dxCommon, type);
		compiler_[type] = std::move(compiler);

		for (BlendMode mode : AllBlendModes()) {
			
			// ペアのキーをｓ作成
			auto key = std::make_pair(type, mode);

			// パイプラインの作成
			auto pipeline = std::make_unique<PipelineStateObjectCommon>();
			pipeline->Create(dxCommon, compiler_[type].get(), type, mode);

			// パイプラインの追加
			pipelines_[key] = std::move(pipeline);
		}
	}
}

///-------------------------------------------/// 
/// パイプラインの登録
///-------------------------------------------///
void PipelineManager::SetPipeline(
	ID3D12GraphicsCommandList * commandList, PipelineType type, BlendMode mode, D3D12_PRIMITIVE_TOPOLOGY topology) {
	// パイプラインの取得
	PipelineStateObjectCommon* pipeline = GetPipeline(type, mode);
	assert(pipeline != nullptr);

	// PSO を設定
	pipeline->SetPSO(commandList);

	//プリミティブトポロジー設定
	commandList->IASetPrimitiveTopology(topology);
}

///-------------------------------------------/// 
/// タイプとモードを取得
///-------------------------------------------///
PipelineStateObjectCommon* PipelineManager::GetPipeline(PipelineType type, BlendMode mode) {
	auto key = std::make_pair(type, mode);
	auto it = pipelines_.find(key);
	if (it != pipelines_.end()) {
		return it->second.get();
	}
	return nullptr;
}

