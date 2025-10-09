#include "SkyBox.h"
// c++
#include <cassert>
#include <fstream>
// Engine
#include "Engine/System/Service/GraphicsResourceGetter.h"
#include "Engine/System/Service/Render.h"
#include "Engine/System/Service/CameraService.h"
// camera
#include "application/Game/Camera/GameCamera.h"
// Math
#include "Math/sMath.h"
#include "Math/MatrixMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
SkyBox::~SkyBox() {}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
/// ===モデル=== ///
const Vector3& SkyBox::GetTranslate() const { return worldTransform_.translate; }
const Quaternion& SkyBox::GetRotate() const { return worldTransform_.rotate; }
const Vector3& SkyBox::GetScale() const { return worldTransform_.scale; }
const Vector4& SkyBox::GetColor() const { return color_; }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
/// ===モデル=== ///
void SkyBox::SetTranslate(const Vector3& position) { worldTransform_.translate = position; }
void SkyBox::SetRotate(const Quaternion& rotate) { worldTransform_.rotate = rotate; }
void SkyBox::SetScale(const Vector3& scale) { worldTransform_.scale = scale; }
void SkyBox::SetColor(const Vector4& color) { color_ = color; }
/// ===Light=== ///
void SkyBox::SetLight(LightType type) { common_->SetLightType(type); }
// LightInfo
void SkyBox::SetLightData(LightInfo light) { light_ = light; }
// 環境マップ
void SkyBox::SetEnviromentMapData(bool flag, float string) {
	enviromentMapInfo_.isEnviromentMap = flag;
	enviromentMapInfo_.strength = string;
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void SkyBox::Initialize(const std::string& fileName, LightType type) {
	/// ===コマンドリストのポインタの取得=== ///
	ID3D12Device* device = GraphicsResourceGetter::GetDXDevice();

	/// ===生成=== ///
	vertex_ = std::make_unique<VertexBuffer3D>();
	index_ = std::make_unique<IndexBuffer3D>();
	common_ = std::make_unique<ObjectCommon>();

	/// ===worldTransform=== ///
	worldTransform_ = { { 100.0f, 100.0f, 100.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
	uvTransform_ = { {1.0f, 1.0f,1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	/// ===vertex=== ///
	// Buffer
	vertex_->Create(device, sizeof(VertexData3D) * kVertexCount);
	vertex_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 頂点情報の設定
	VertexDataWrite();
	// view
	vertexBufferView_.BufferLocation = vertex_->GetBuffer()->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData3D) * kVertexCount;
	vertexBufferView_.StrideInBytes = sizeof(VertexData3D);


	/// ===index=== ///
	index_->Create(device, sizeof(uint32_t) * kIndexCount);
	index_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	// インデックスデータ
	uint32_t indices[kIndexCount] = {
		0,1,2, 2,1,3,        // 右
		4,5,6, 6,5,7,        // 左
		8,9,10,10,9,11,      // 前
		12,13,14,14,13,15,   // 後
		16,18,17,17,18,19,   // 上（修正済）
		20,22,21,21,22,23    // 下（修正済）
	};
	std::memcpy(indexData_, indices, sizeof(indices));
	// view
	indexBufferView_.BufferLocation = index_->GetBuffer()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * kIndexCount;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	/// ===Common=== ///
	common_->Initialize(device, type);

	// テクスチャパス保存
	textureFilePath_ = fileName;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void SkyBox::Update() {
	/// ===カメラの設定=== ///
	camera_ = CameraService::GetActiveCamera().get();

	/// ===データの書き込み=== ///
	VertexDataWrite();
	MateialDataWrite();
	TransformDataWrite();
	LightDataWrite();
	CameraDataWrite(); 
	EnviromentMapDataWrite();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void SkyBox::Draw(BlendMode mode) {
	/// ===コマンドリストのポインタの取得=== ///
	ID3D12GraphicsCommandList* commandList = GraphicsResourceGetter::GetDXCommandList();

	/// ===コマンドリストに設定=== ///
	// PSOの設定
	Render::SetPSO(commandList, PipelineType::PrimitiveSkyBox, mode);
	// Viewの設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);
	// 共通部の設定
	common_->Bind(commandList);
	// テクスチャの設定
	Render::SetGraphicsRootDescriptorTable(commandList, 2, textureFilePath_);
	// 描画（Drawコール）
	commandList->DrawIndexedInstanced(kIndexCount, 1, 0, 0, 0);
}

///-------------------------------------------/// 
/// VertrexDataの書き込み
///-------------------------------------------///
void SkyBox::VertexDataWrite() {
	// 右面。描画インデックスは[0,1,2][2,1,3]で内側を向く
	vertexData_[0].position = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertexData_[1].position = { 1.0f, 1.0f, -1.0f, 1.0f };
	vertexData_[2].position = { 1.0f, -1.0f, 1.0f, 1.0f };
	vertexData_[3].position = { 1.0f, -1.0f, -1.0f, 1.0f };
	// 左面。描画インデックスは[4,5,6][6,5,7]
	vertexData_[4].position = { -1.0f, 1.0f, -1.0f, 1.0f };
	vertexData_[5].position = { -1.0f, 1.0f, 1.0f, 1.0f };
	vertexData_[6].position = { -1.0f, -1.0f, -1.0f, 1.0f };
	vertexData_[7].position = { -1.0f, -1.0f, 1.0f, 1.0f };
	// 前面。描画インデックスは[8,9,10][10,9,11]
	vertexData_[8].position = { -1.0f, 1.0f, 1.0f, 1.0f };
	vertexData_[9].position = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertexData_[10].position = { -1.0f, -1.0f, 1.0f, 1.0f };
	vertexData_[11].position = { 1.0f, -1.0f, 1.0f, 1.0f };
	// 後面
	vertexData_[12].position = { 1.0f, 1.0f, -1.0f, 1.0f };
	vertexData_[13].position = { -1.0f, 1.0f, -1.0f, 1.0f };
	vertexData_[14].position = { 1.0f, -1.0f, -1.0f, 1.0f };
	vertexData_[15].position = { -1.0f, -1.0f, -1.0f, 1.0f };
	// 上面
	vertexData_[16].position = { -1.0f, 1.0f, -1.0f, 1.0f };
	vertexData_[17].position = { 1.0f, 1.0f, -1.0f, 1.0f };
	vertexData_[18].position = { -1.0f, 1.0f, 1.0f, 1.0f };
	vertexData_[19].position = { 1.0f, 1.0f, 1.0f, 1.0f };
	// 下面
	vertexData_[20].position = { -1.0f, -1.0f, 1.0f, 1.0f };
	vertexData_[21].position = { 1.0f, -1.0f, 1.0f, 1.0f };
	vertexData_[22].position = { -1.0f, -1.0f, -1.0f, 1.0f };
	vertexData_[23].position = { 1.0f, -1.0f, -1.0f, 1.0f };
}

///-------------------------------------------/// 
/// MaterialDataの書き込み
///-------------------------------------------///
void SkyBox::MateialDataWrite() {
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
/// WVPDataの書き込み
///-------------------------------------------///
void SkyBox::TransformDataWrite() {
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
/// LightDataの書き込み
///-------------------------------------------///
void SkyBox::LightDataWrite() {
	common_->SetDirectionLight(
		light_.directional.color,
		light_.directional.direction,
		light_.directional.intensity
	);
	common_->SetPointLightData(
		light_.point.color,
		light_.point.position,
		light_.point.intensity,
		light_.point.radius,
		light_.point.decay
	);
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
/// CameraDataの書き込み
///-------------------------------------------///
void SkyBox::CameraDataWrite() {
	common_->SetCameraForGPU(camera_->GetTranslate());
}

///-------------------------------------------/// 
/// 環境マップの書き込み
///-------------------------------------------///
void SkyBox::EnviromentMapDataWrite() {
	common_->SetEnviromentMapData(enviromentMapInfo_.isEnviromentMap, enviromentMapInfo_.strength);
}