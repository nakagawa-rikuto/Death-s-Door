#include "VertexBuffer2D.h"

#include <cassert>

VertexBuffer2D::VertexBuffer2D() {}
VertexBuffer2D::~VertexBuffer2D() { buffer_.Reset(); }

///-------------------------------------------/// 
/// リソースの生成
///-------------------------------------------///
void VertexBuffer2D::Create(ID3D12Device* device, size_t sizeInBytes) {
	buffer_ = CreateBufferResourceComPtr(device, sizeInBytes);
}

///-------------------------------------------/// 
/// データの書き込み
///-------------------------------------------///
void VertexBuffer2D::WriteData(VertexData2D* data) {
	// 書き込むためのアドレスを取得
	buffer_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	data_ = data;
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ID3D12Resource* VertexBuffer2D::GetBuffer() { return buffer_.Get(); }
VertexData2D* VertexBuffer2D::GetData() {return data_;}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void VertexBuffer2D::SetData(VertexData2D* data) { data_ = data; }


