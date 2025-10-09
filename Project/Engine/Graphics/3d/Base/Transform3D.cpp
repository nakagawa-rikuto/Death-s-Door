#include "Transform3D.h"

#include <cassert>

///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
Transform3D::Transform3D() {}
Transform3D::~Transform3D() { buffer_.Reset(); }

///-------------------------------------------/// 
///
///-------------------------------------------///
void Transform3D::Create(ID3D12Device* device, size_t sizeInBytes) {
	buffer_ = CreateBufferResourceComPtr(device, sizeInBytes);
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ID3D12Resource* Transform3D::GetBuffer() { return buffer_.Get(); }
TransformationMatrix3D* Transform3D::GetData() { return data_; }