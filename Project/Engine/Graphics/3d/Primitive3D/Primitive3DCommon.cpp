#include "Primitive3DCommon.h"
// Math
#include "Math/MatrixMath.h"
// Service
#include "Engine/System/Service/Render.h"
#include "Engine/System/Service/CameraService.h"
// camera
#include "application/Game/Camera/GameCamera.h"

///-------------------------------------------/// 
/// コンストラクタ・デストラクタ
///-------------------------------------------///
Primitive3DCommon::Primitive3DCommon() = default;
Primitive3DCommon::~Primitive3DCommon() {
	vertex_.reset();
	index_.reset();
	common_.reset();
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void Primitive3DCommon::SetLightType(LightType type) { common_->SetLightType(type); }

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Primitive3DCommon::Create(ID3D12Device* device, LightType type) {
	/// ===初期化時の設定=== ///
	worldTransform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
	uvTransform_ = { {1.0f, 1.0f,1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	light_ = {
		40.0f,
		{{ 1.0f, 1.0f, 1.0f, 1.0f } , { 0.0f, -1.0f, 0.0f } ,1.0f},
		{{ 1.0f, 1.0f, 1.0f, 1.0f } , { 0.0f, 0.0f, 0.0f } , 1.0f, 0.0f, 0.0f},
		{{ 1.0f, 1.0f, 1.0f, 1.0f } , { 0.0f, 0.0f, 0.0f } , 0.0f, { 0.0f, 0.0f, 0.0f } , 0.0f, 0.0f, 0.0f}
	};
	enviromentMapInfo_ = {
		"skyBox",
		false,
		1.0f
	};

	vertex_ = std::make_unique<VertexBuffer3D>();
	index_ = std::make_unique<IndexBuffer3D>();
	common_ = std::make_shared<ObjectCommon>();

	/// ===Common=== ///
	common_->Initialize(device, type);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Primitive3DCommon::Update() {
	/// ===カメラの設定=== ///
	camera_ = CameraService::GetActiveCamera().get();

	// MaterialDataの書き込み
	MateialDataWrite();
	// Transform情報の書き込み
	TransformDataWrite();
	// Lightの書き込み
	LightDataWrite();
	// Cameraの書き込み
	CameraDataWrite();
	// 環境マップの書き込み
	EnviromentMapDataWrite();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void Primitive3DCommon::Bind(ID3D12GraphicsCommandList* commandList) {

	/// ===コマンドリストに設定=== ///
	// Commonの設定
	common_->Bind(commandList);
}

///-------------------------------------------/// 
/// MaterialDataの書き込み
///-------------------------------------------///
void Primitive3DCommon::MateialDataWrite() {
	/// ===Matrixの作成=== ///
	Matrix4x4 uvTransformMatrix = Math::MakeScaleMatrix(uvTransform_.scale);
	Matrix4x4 uvTransformMatrixMultiply = Multiply(uvTransformMatrix, Math::MakeRotateZMatrix(uvTransform_.rotate.z));
	uvTransformMatrixMultiply = Multiply(uvTransformMatrixMultiply, Math::MakeTranslateMatrix(uvTransform_.translate));

	/// ===値の代入=== ///
	common_->SetMatiarlData(
		color_,
		light_.shininess,
		uvTransformMatrixMultiply
	);
}

///-------------------------------------------/// 
/// Transform情報の書き込み
///-------------------------------------------///
void Primitive3DCommon::TransformDataWrite() {

	Matrix4x4 worldMatrix = Math::MakeAffineQuaternionMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	Matrix4x4 worldViewProjectionMatrix;

	/// ===Matrixの作成=== ///
	const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
	worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);

	/// ===値の代入=== ///
	common_->SetTransformData(
		worldViewProjectionMatrix,
		worldMatrix,
		Math::Inverse4x4(worldMatrix)
	);

}

///-------------------------------------------///  
///　ライトの書き込み
///-------------------------------------------///
void Primitive3DCommon::LightDataWrite() {

	// DirectionalLightの書き込み
	common_->SetDirectionLight(
		light_.directional.color,
		light_.directional.direction,
		light_.directional.intensity
	);

	// PointLightの書き込み
	common_->SetPointLightData(
		light_.point.color,
		light_.point.position,
		light_.point.intensity,
		light_.point.radius,
		light_.point.decay
	);

	// SpotLightの書き込み
	common_->SetSpotLightData(
		light_.spot.color,
		light_.spot.position,
		light_.spot.direction,
		light_.spot.intensity,
		light_.spot.distance,
		light_.spot.decay,
		light_.spot.cosAngle
	);
}

///-------------------------------------------/// 
/// カメラの書き込み
///-------------------------------------------///
void Primitive3DCommon::CameraDataWrite() {
	common_->SetCameraForGPU(camera_->GetTranslate()); // カメラの位置をワールド座標系で取得
}

///-------------------------------------------/// 
/// 環境マップの書き込み
///-------------------------------------------///
void Primitive3DCommon::EnviromentMapDataWrite() {
	common_->SetEnviromentMapData(enviromentMapInfo_.isEnviromentMap, enviromentMapInfo_.strength);
}