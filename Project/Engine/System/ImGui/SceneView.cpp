#include "SceneView.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#endif // USE_IMGUI

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void SceneView::SetTextureHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle) { textureHandle_ = handle; }

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void SceneView::Draw() {
#ifdef USE_IMGUI
    ImGui::Begin("Scene");
    ImVec2 size = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)textureHandle_.ptr, size);
    ImGui::End();
#endif // USE_IMGUI
}
