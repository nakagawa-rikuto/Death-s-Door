#include "IndexBuffer3D.h"

#include <cassert>

IndexBuffer3D::IndexBuffer3D() {}
IndexBuffer3D::~IndexBuffer3D() { buffer_.Reset(); }

///-------------------------------------------/// 
///リソースの作成
///-------------------------------------------///
void IndexBuffer3D::Create(ID3D12Device* device, size_t sizeInBytes) {
	buffer_ = CreateBufferResourceComPtr(device, sizeInBytes);
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ID3D12Resource* IndexBuffer3D::GetBuffer() { return buffer_.Get(); }
uint32_t* IndexBuffer3D::GetData() { return data_; }