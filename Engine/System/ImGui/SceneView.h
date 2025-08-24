#pragma once
/// ===Include=== ///
#include <d3dx12.h>

///-------------------------------------------/// 
/// SceneView
///-------------------------------------------///
class SceneView {
public:

	void SetTextureHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle);

	void Draw();

private:
	D3D12_GPU_DESCRIPTOR_HANDLE textureHandle_{};
};

