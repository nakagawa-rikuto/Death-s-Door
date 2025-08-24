#include "Material2D.h"
// Math
#include "Math/sMath.h"

///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
Material2D::Material2D() {}
Material2D::~Material2D() { buffer_.Reset(); }

///-------------------------------------------/// 
/// リソースの生成
///-------------------------------------------///
void Material2D::Create(ID3D12Device* device, size_t sizeInBytes) {
	buffer_ = CreateBufferResourceComPtr(device, sizeInBytes);
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
ID3D12Resource* Material2D::GetBuffer() {return buffer_.Get();}
MaterialData2D* Material2D::GetData() {return data_;}
