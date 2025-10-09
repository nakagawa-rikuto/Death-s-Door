#include "GraphicsResourceGetter.h"
// c++
#include <cassert>
// Engine
#include "Engine/Core/WinApp.h"
#include "Engine/Core/DXCommon.h"
// Manager
#include "Engine/System/Managers/SRVManager.h"
#include "Engine/System/Managers/RTVManager.h"
#include "Engine/System/Managers/DSVManager.h"
#include "Engine/System/Managers/TextureManager.h"
#include "Engine/System/Managers/ModelManager.h"
#include "Engine/System/Managers/AnimationManager.h"
#include "Engine/System/Managers/LevelManager.h"
// ServiceLocator
#include "ServiceLocator.h"

///-------------------------------------------/// 
/// DescriptorHandle
///-------------------------------------------///
// RTV
D3D12_CPU_DESCRIPTOR_HANDLE GraphicsResourceGetter::GetRTVCPUDescriptorHandle(uint32_t index) {
	return ServiceLocator::GetRTVManager()->GetCPUDescriptorHandle(index);
}
D3D12_GPU_DESCRIPTOR_HANDLE GraphicsResourceGetter::GetRTVGPUDescriptorHandle(uint32_t index) {
	return ServiceLocator::GetRTVManager()->GetGPUDescriptorHandle(index);
}
// DSV
D3D12_CPU_DESCRIPTOR_HANDLE GraphicsResourceGetter::GetDSVCPUDescriptorHandle(uint32_t index) {
	return ServiceLocator::GetDSVManager()->GetCPUDescriptorHandle(index);
}
D3D12_GPU_DESCRIPTOR_HANDLE GraphicsResourceGetter::GetDSVGPUDescriptorHandle(uint32_t index) {
	return ServiceLocator::GetDSVManager()->GetGPUDescriptorHandle(index);
}
// SRV
D3D12_CPU_DESCRIPTOR_HANDLE GraphicsResourceGetter::GetSRVCPUDescriptorHandle(uint32_t index) {
	return ServiceLocator::GetSRVManager()->GetCPUDescriptorHandle(index);
}
D3D12_GPU_DESCRIPTOR_HANDLE GraphicsResourceGetter::GetSRVGPUDescriptorHandle(uint32_t index) {
	return ServiceLocator::GetSRVManager()->GetGPUDescriptorHandle(index);
}

///-------------------------------------------/// 
/// WinApp
///-------------------------------------------///
const int GraphicsResourceGetter::GetWindowWidth() {
	return ServiceLocator::GetWinApp()->GetWindowWidth();
}
const int GraphicsResourceGetter::GetWindowHeight() {
	return ServiceLocator::GetWinApp()->GetWindowHeight();
}

///-------------------------------------------/// 
/// Data
///-------------------------------------------///
// メタデータの取得
const DirectX::TexMetadata& GraphicsResourceGetter::GetMetaData(const std::string& key) {
	return ServiceLocator::GetTextureManager()->GetMetaData(key);
}
// モデルデータの取得
ModelData GraphicsResourceGetter::GetModelData(const std::string& directorPath) {
	return ServiceLocator::GetModelManager()->GetModelData(directorPath);
}
// アニメーションの取得
std::map<std::string, Animation> GraphicsResourceGetter::GetAnimationData(const std::string& directorPath) {
	return ServiceLocator::GetAnimationManager()->GetAnimation(directorPath);
}
// LevelDataの取得
LevelData* GraphicsResourceGetter::GetLevelData(const std::string& file_name) {
	return ServiceLocator::GetLevelManager()->GetLevelData(file_name);
}


///-------------------------------------------/// 
/// DXCommon
///-------------------------------------------///
// Deviceの取得
ID3D12Device* GraphicsResourceGetter::GetDXDevice() {
	return ServiceLocator::GetDXCommon()->GetDevice();
}
// CommandListの取得
ID3D12GraphicsCommandList* GraphicsResourceGetter::GetDXCommandList() {
	return ServiceLocator::GetDXCommon()->GetCommandList();
}