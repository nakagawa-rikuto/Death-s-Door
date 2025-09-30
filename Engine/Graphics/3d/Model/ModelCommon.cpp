#include "ModelCommon.h"
// Math
#include "Math/MatrixMath.h"
// Service
#include "Engine/System/Service/Render.h"
#include "Engine/System/Service/CameraService.h"
// Camera
#include "application/Game/Camera/GameCamera.h"

///-------------------------------------------/// 
/// コンストラクタ・デストラクタ
///-------------------------------------------///
ModelCommon::ModelCommon() = default;
ModelCommon::~ModelCommon() {
	// 親子関係の解除
	ClearParent();

	// 解放
	vertex_.reset();
	index_.reset();
	common_.reset();
}

///-------------------------------------------/// 
/// 親子関係
///-------------------------------------------///
// 親の設定
void ModelCommon::SetParent(ModelCommon* parent) { parent_ = parent; }
// 親の解除
void ModelCommon::ClearParent() { SetParent(nullptr); }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void ModelCommon::SetLightType(LightType type) {common_->SetLightType(type);}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void ModelCommon::Create(ID3D12Device* device, LightType type) {
	/// ===初期化時の設定=== ///
	worldTransform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
	uvTransform_ = { {1.0f, 1.0f,1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
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

	/// ===生成=== ///
	vertex_ = std::make_unique<VertexBuffer3D>();
	index_ = std::make_unique<IndexBuffer3D>();
	common_ = std::make_unique<ObjectCommon>();

	/// ===vertex=== ///
	// Buffer
	vertex_->Create(device, sizeof(VertexData3D) * modelData_.vertices.size());
	vertex_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// メモリコピー
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData3D) * modelData_.vertices.size());
	// view
	vertexBufferView_.BufferLocation = vertex_->GetBuffer()->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData3D) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData3D);

	/// ===index=== ///
	index_->Create(device, sizeof(uint32_t) * modelData_.indices.size());
	index_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	// メモリコピー
	std::memcpy(indexData_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
	// view
	indexBufferView_.BufferLocation = index_->GetBuffer()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	/// ===Common=== ///
	common_->Initialize(device, type);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void ModelCommon::Update() {
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
void ModelCommon::Bind(ID3D12GraphicsCommandList* commandList) {

	/// ===コマンドリストに設定=== ///
	// Commonの設定
	common_->Bind(commandList);
}

///-------------------------------------------/// 
/// MaterialDataの書き込み
///-------------------------------------------///
void ModelCommon::MateialDataWrite() {
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
void ModelCommon::TransformDataWrite() {

	Matrix4x4 worldMatrix = Math::MakeAffineQuaternionMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	Matrix4x4 worldViewProjectionMatrix;

	/// ===親の確認=== ///
	if (parent_) {
		// 親のワールド行列
		Matrix4x4 parentWorldMatrix = Math::MakeAffineQuaternionMatrix(parent_->worldTransform_.scale, parent_->worldTransform_.rotate, parent_->worldTransform_.translate);
		worldMatrix = Multiply(worldMatrix, parentWorldMatrix);
	}

	/// ===Matrixの作成=== ///
	const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
	worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);

	/// ===値の代入=== ///
	common_->SetTransformData(
		worldViewProjectionMatrix,
		Multiply(modelData_.rootNode.localMatrix, worldMatrix),
		Math::Inverse4x4(worldMatrix)
	);

}

///-------------------------------------------///  
///　ライトの書き込み
///-------------------------------------------///
void ModelCommon::LightDataWrite() {
	
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
void ModelCommon::CameraDataWrite() {
	common_->SetCameraForGPU(camera_->GetTranslate()); // カメラの位置をワールド座標系で取得
}

///-------------------------------------------/// 
/// 環境マップの書き込み
///-------------------------------------------///
void ModelCommon::EnviromentMapDataWrite() {
	common_->SetEnviromentMapData(enviromentMapInfo_.isEnviromentMap, enviromentMapInfo_.strength);
}