#include "ObjectCommon.h"
// Math
#include "Math/MatrixMath.h"

///-------------------------------------------/// 
/// コンストラクタ・デストラクタ
///-------------------------------------------///
ObjectCommon::ObjectCommon() = default;
ObjectCommon::~ObjectCommon() {
	material_.reset();
	wvp_.reset();
	directionallight_.reset();
	camera3D_.reset();
	pointLight_.reset();
	spotLight_.reset();
	enviromentMap_.reset();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
LightType ObjectCommon::GetLightType() const {
	if (materialData_->enableLighting == 1) {
		return LightType::Lambert;
	} else if (materialData_->enableLighting == 2) {
		return LightType::HalfLambert;
	} else if (materialData_->enableLighting == 3) {
		return LightType::PointLight;
	} else if (materialData_->enableLighting == 4) {
		return LightType::SpotLight;
	} else {
		return LightType::None;
	}
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// mateial
void ObjectCommon::SetMatiarlData(const Vector4& color, const float& shininess, const Matrix4x4& uvTransform) {
	materialData_->color = color;
	materialData_->shininess = shininess;
	materialData_->uvTransform = uvTransform;
}
// wvp
void ObjectCommon::SetTransformData(const Matrix4x4& WVP, const Matrix4x4& World, const Matrix4x4& WorldInverseTranspose) {
	wvpMatrixData_->WVP = WVP;
	wvpMatrixData_->World = World;
	wvpMatrixData_->WorldInverseTranspose = WorldInverseTranspose;
}
// LightType
void ObjectCommon::SetLightType(LightType type) {
	if (type == LightType::Lambert) {
		materialData_->enableLighting = 1;
	} else if (type == LightType::HalfLambert) {
		materialData_->enableLighting = 2;
	} else if (type == LightType::PointLight) {
		materialData_->enableLighting = 3;
	} else if (type == LightType::SpotLight) {
		materialData_->enableLighting = 4;
	} else {
		materialData_->enableLighting = 0;
	}
}
// DirectionlLight
void ObjectCommon::SetDirectionLight(const Vector4& color, const Vector3& direction, const float& intensity) {
	directionalLightData_->color = color;
	directionalLightData_->direction = direction;
	directionalLightData_->intensity = intensity;
}
// PointLight
void ObjectCommon::SetPointLightData(const Vector4& color, const Vector3& position, const float& intensity, const float& radius, const float& decay) {
	pointLightData_->color = color;
	pointLightData_->position = position;
	pointLightData_->intensity = intensity;
	pointLightData_->radius = radius;
	pointLightData_->decay = decay;
}
// SpotLight
void ObjectCommon::SetSpotLightData(const Vector4& color, const Vector3& position, const Vector3& direction, const float& intensity, const float& distance, const float& decay, const float& cosAngle) {
	spotLightData_->color = color;
	spotLightData_->position = position;
	spotLightData_->direction = direction;
	spotLightData_->intensity = intensity;
	spotLightData_->distance = distance;
	spotLightData_->decay = decay;
	spotLightData_->cosAngle = cosAngle;
}
// cameraForGPU
void ObjectCommon::SetCameraForGPU(const Vector3& translate) {
	cameraForGPUData_->worldPosition = translate;
}
// EnviromentMap
void ObjectCommon::SetEnviromentMapData(bool enable, float strength) {
	enviromentMapData_->enable = enable;
	enviromentMapData_->strength = strength;
}


///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void ObjectCommon::Initialize(ID3D12Device* device, LightType type) {

	/// ===生成=== ///
	material_ = std::make_unique<Material3D>();
	wvp_ = std::make_unique<Transform3D>();
	camera3D_ = std::make_unique<BufferBase>();
	directionallight_ = std::make_unique<BufferBase>();
	pointLight_ = std::make_unique<BufferBase>();
	spotLight_ = std::make_unique<BufferBase>();
	enviromentMap_ = std::make_unique<BufferBase>();

	/// ===Material=== ///
	// buffer
	material_->Create(device, sizeof(MaterialData3D));
	material_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// Data書き込み
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (type == LightType::Lambert) {
		materialData_->enableLighting = 1;
	} else if (type == LightType::HalfLambert) {
		materialData_->enableLighting = 2;
	} else if (type == LightType::PointLight) {
		materialData_->enableLighting = 3;
	} else if (type == LightType::SpotLight) {
		materialData_->enableLighting = 4;
	} else {
		materialData_->enableLighting = 0;
	}
	materialData_->shininess = 10.0f;
	materialData_->uvTransform = Math::MakeIdentity4x4();

	/// ===wvp=== ///
	// buffer
	wvp_->Create(device, sizeof(TransformationMatrix3D));
	wvp_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&wvpMatrixData_));
	// Dataの書き込み
	wvpMatrixData_->WVP = Math::MakeIdentity4x4();
	wvpMatrixData_->World = Math::MakeIdentity4x4();
	wvpMatrixData_->WorldInverseTranspose = Math::Inverse4x4(wvpMatrixData_->World);

	/// ===DirectionalLight=== ///
	directionallight_->Create(device, sizeof(DirectionalLight));
	directionallight_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData_->intensity = 1.0f;

	/// ===Camera=== ///
	camera3D_->Create(device, sizeof(CameraForGPU));
	camera3D_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPUData_));
	cameraForGPUData_->worldPosition = { 0.0f, 4.0f, -10.0f };

	/// ===PointLight=== ///
	pointLight_->Create(device, sizeof(PointLight));
	pointLight_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	pointLightData_->position = { 0.0f, 0.0f, 0.0f };
	pointLightData_->intensity = 1.0f;

	/// ===SpotLight=== ///
	spotLight_->Create(device, sizeof(SpotLight));
	spotLight_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
	spotLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	spotLightData_->position = { 0.0f, 0.0f, 0.0f };
	spotLightData_->intensity = 1.0f;
	spotLightData_->direction = { 0.0f, 0.0f, 0.0f };
	spotLightData_->distance = 0.0f;
	spotLightData_->decay = 0.0f;
	spotLightData_->cosAngle = 0.0f;

	/// ===EnviromentMap=== ///
	enviromentMap_->Create(device, sizeof(EnviromentMap));
	enviromentMap_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&enviromentMapData_));
	enviromentMapData_->enable = 0; // 環境マップは初期状態では無効化
	enviromentMapData_->strength = 1.0f; // 環境マップの強度は1.0fに設定
}


///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void ObjectCommon::Bind(ID3D12GraphicsCommandList* commandList) {

	/// ===コマンドリストに設定=== ///
	// MaterialBufferの設定
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetBuffer()->GetGPUVirtualAddress());
	// wvpMatrixBufferの設定
	commandList->SetGraphicsRootConstantBufferView(1, wvp_->GetBuffer()->GetGPUVirtualAddress());
	// DirectionlLightの設定
	commandList->SetGraphicsRootConstantBufferView(4, directionallight_->GetBuffer()->GetGPUVirtualAddress());
	// CameraBufferの設定
	commandList->SetGraphicsRootConstantBufferView(5, camera3D_->GetBuffer()->GetGPUVirtualAddress());
	// PointLight
	commandList->SetGraphicsRootConstantBufferView(6, pointLight_->GetBuffer()->GetGPUVirtualAddress());
	// SpotLight
	commandList->SetGraphicsRootConstantBufferView(7, spotLight_->GetBuffer()->GetGPUVirtualAddress());
	// EnviromentMap
	commandList->SetGraphicsRootConstantBufferView(8, enviromentMap_->GetBuffer()->GetGPUVirtualAddress());
}