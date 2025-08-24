#include "Transform2D.h"

#include <cassert>

Transform2D::Transform2D() {}
Transform2D::~Transform2D() { buffer_.Reset(); }

///-------------------------------------------/// 
/// リソースの作成
///-------------------------------------------///
void Transform2D::Create(ID3D12Device* device, size_t sizeInBytes) {
	buffer_ = CreateBufferResourceComPtr(device, sizeInBytes);
}

///-------------------------------------------/// 
/// データの書き込み
///-------------------------------------------///
void Transform2D::WriteData(TransformationMatrix2D* data) {
	// 書き込むためのアドレスを取得
	buffer_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	data_ = data;
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ID3D12Resource* Transform2D::GetBuffer() { return buffer_.Get(); }
TransformationMatrix2D* Transform2D::GetData() { return data_; }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void Transform2D::SetData(TransformationMatrix2D* data) { data_ = data; }


