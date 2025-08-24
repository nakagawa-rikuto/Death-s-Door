#pragma once
#include "Engine/Core/ComPtr.h"
#include "Engine/DataInfo/CData.h"

/// <summary>
/// CreateBufferResource
/// </summary>
/// <param name="device"></param>
/// <param name="sizeInBytes"></param>
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

/// <summary>
/// CreateBufferResource(ComPtr)
/// </summary>
/// <param name="device"></param>
/// <param name="sizeInBytes"></param>
ComPtr<ID3D12Resource> CreateBufferResourceComPtr(ID3D12Device* device, size_t sizeInBytes);

