#include "Object2DCommon.h"
// Math
#include "Math/MatrixMath.h"

///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
Object2DCommon::Object2DCommon() = default;
Object2DCommon::~Object2DCommon() {
	material_.reset();
	wvp_.reset();
}

///-------------------------------------------/// 
/// MaterialBufferの初期化
///-------------------------------------------///
void Object2DCommon::MaterialInitialize(ID3D12Device* device, uint32_t size) {
	// 生成
	material_ = std::make_unique<Material2D>();
	// buffer
	material_->Create(device, sizeof(MaterialData2D) * size);
	material_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->uvTransform = Math::MakeIdentity4x4();
}

///-------------------------------------------/// 
/// WVPBufferの初期化
///-------------------------------------------///
void Object2DCommon::WVPMatrixInitialize(ID3D12Device* device) {
	// 生成
	wvp_ = std::make_unique<Transform2D>();
	// buffer
	wvp_->Create(device, sizeof(TransformationMatrix2D));
	wvp_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&wvpMatrixData_));
	wvpMatrixData_->WVP = Math::MakeIdentity4x4();
}

///-------------------------------------------/// 
/// CommandListの設定
///-------------------------------------------///
void Object2DCommon::Bind(ID3D12GraphicsCommandList* commandList) {

	// Materialの設定
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetBuffer()->GetGPUVirtualAddress());
	// wvpMatrixBufferの設定
	commandList->SetGraphicsRootConstantBufferView(1, wvp_->GetBuffer()->GetGPUVirtualAddress());
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void Object2DCommon::SetMateiralData(const Vector4& color, const Matrix4x4& uvTransform) {
	materialData_->color = color;
	materialData_->uvTransform = uvTransform;
}
void Object2DCommon::SetWVPData(const Matrix4x4& WVP) {
	wvpMatrixData_->WVP = WVP;
}
