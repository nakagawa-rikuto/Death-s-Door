#include "IndexBuffer2D.h"

#include <cassert>

IndexBuffer2D::IndexBuffer2D() {}
IndexBuffer2D::~IndexBuffer2D() { buffer_.Reset(); }

///-------------------------------------------/// 
///リソースの作成
///-------------------------------------------///
void IndexBuffer2D::Create(ID3D12Device* device, size_t sizeInBytes) {
	buffer_ = CreateBufferResourceComPtr(device, sizeInBytes);
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ID3D12Resource* IndexBuffer2D::GetBuffer() { return buffer_.Get(); }
uint32_t* IndexBuffer2D::GetData() { return data_; }
