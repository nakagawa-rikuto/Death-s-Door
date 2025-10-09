#include "BufferBase.h"

#include <cassert>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
BufferBase::~BufferBase() { buffer_.Reset(); }

///-------------------------------------------/// 
/// リソースの生成
///-------------------------------------------///
void BufferBase::Create(ID3D12Device* device, size_t sizeInBytes) {
	buffer_ = CreateBufferResourceComPtr(device, sizeInBytes);
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ID3D12Resource* BufferBase::GetBuffer() { return buffer_.Get(); }