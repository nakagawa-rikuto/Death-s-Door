#include "Render.h"
// Manager
#include "Engine/System/Managers/PiplineManager.h"
#include "Engine/System/Managers/TextureManager.h"
// Locator
#include "Locator.h"

namespace Service {
	///-------------------------------------------/// 
	/// Pipeline
	///-------------------------------------------///
	// PSOの取得
	void Render::SetPSO(ID3D12GraphicsCommandList* commandList, MiiEngine::PipelineType type, MiiEngine::BlendMode mode, D3D12_PRIMITIVE_TOPOLOGY topology) {
		Locator::GetPipelineManager()->SetPipeline(commandList, type, mode, topology);
	}

	///-------------------------------------------/// 
	/// CSPSOの設定 
	///-------------------------------------------///
	void Render::SetCSPSO(ID3D12GraphicsCommandList* commandList, MiiEngine::CSPipelineType type, const std::wstring& kernelName) {
		Locator::GetPipelineManager()->SetCSPipeline(commandList, type, kernelName);
	}

	///-------------------------------------------/// 
	/// Texture
	///-------------------------------------------///
	// SRVインデックス開始番号の取得
	void Render::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT RootParameterIndex, const std::string& key) {
		Locator::GetTextureManager()->SetGraphicsRootDescriptorTable(commandList, RootParameterIndex, key);
	}
}
