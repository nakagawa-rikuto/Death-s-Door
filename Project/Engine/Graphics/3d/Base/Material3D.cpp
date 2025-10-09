#include "Material3D.h"

#include <cassert>

Material3D::Material3D() {}
Material3D::~Material3D() { buffer_.Reset(); }

///-------------------------------------------/// 
/// リソースの生成
///-------------------------------------------///
void Material3D::Create(ID3D12Device* device, size_t sizeInBytes) {
	buffer_ = CreateBufferResourceComPtr(device, sizeInBytes);
}

///-------------------------------------------/// 
/// データの書き込み
///-------------------------------------------///
void Material3D::WriteData(MaterialData3D* material) {
	// 書き込むためのアドレスを取得
	buffer_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	// 色の書き込み
	data_->color = material->color;
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ID3D12Resource* Material3D::GetBuffer() { return buffer_.Get(); }

