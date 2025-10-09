#include "VertexBuffer3D.h"

#include <cassert>

VertexBuffer3D::VertexBuffer3D() {}
VertexBuffer3D::~VertexBuffer3D() { buffer_.Reset(); }

///-------------------------------------------/// 
/// リソースの生成
///-------------------------------------------///
void VertexBuffer3D::Create(ID3D12Device* device, size_t sizeInBytes) {
	buffer_ = CreateBufferResourceComPtr(device, sizeInBytes);
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ID3D12Resource* VertexBuffer3D::GetBuffer() { return buffer_.Get(); }
VertexData3D* VertexBuffer3D::GetData() { return data_; }