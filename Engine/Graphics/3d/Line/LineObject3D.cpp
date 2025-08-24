#include "LineObject3D.h"
// Service
#include "Engine/System/Service/GraphicsResourceGetter.h"
#include "Engine/System/Service/Render.h"
#include "Engine/System/Service/CameraService.h"
// Camera
#include "application/Game/Camera/camera.h"
// DXCommon
#include "Engine/Core/DXCommon.h"
// Math
#include "Math/sMath.h"
#include "Math/MatrixMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
LineObject3D::~LineObject3D() {
	vertex_.reset();
	wvp_.reset();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
const std::vector<Vector3> LineObject3D::GetSphereData() { return spheres_; }

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void LineObject3D::Initialize(ID3D12Device* device) {
	
	/// ===生成=== ///
	vertex_ = std::make_unique<VertexBuffer3D>();
	wvp_ = std::make_unique<Transform3D>();

	/// ===WorldTransform=== ///
	worldTransform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
	cameraTransform_ = { {1.0f, 1.0f,1.0f}, {0.3f, 0.0f, 0.0f}, {0.0f, 4.0f, -10.0f} };

	/// ===vertex=== ///
	// Buffer
	vertex_->Create(device, sizeof(LineVertexData3D) * kLineVertexCount_ * kMaxLineCount_);
	vertex_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// View
	vertexBufferView_.BufferLocation = vertex_->GetBuffer()->GetGPUVirtualAddress();
	vertexBufferView_.StrideInBytes = sizeof(LineVertexData3D);
	vertexBufferView_.SizeInBytes = sizeof(LineVertexData3D) * kLineVertexCount_ * kMaxLineCount_;
	// 
	SphereVertexData();

	/// ===WVP=== ///
	wvp_->Create(device, sizeof(LineTransformMatrixData3D));
	wvp_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	wvpData_->WVP = Math::MakeIdentity4x4();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void LineObject3D::Update() {
	
	/// ===カメラの設定=== ///
	camera_ = CameraService::GetActiveCamera().get();

	/// ===Matrixの作成=== ///
	const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
	Matrix4x4 worldViewProjectionMatrix = viewProjectionMatrix;

	// Dataの代入
	wvpData_->WVP = worldViewProjectionMatrix;
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void LineObject3D::Draw() {
	/// ===コマンドリストのポインタの取得=== ///
	ID3D12GraphicsCommandList* commandList = GraphicsResourceGetter::GetDXCommandList();

	/// ===コマンドリストに設定=== ///
	// PSOの設定
	Render::SetPSO(commandList, PipelineType::Line3D, BlendMode::KBlendModeNormal, D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// vertexBufferの設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	// wvpMatrixBufferの設定
	commandList->SetGraphicsRootConstantBufferView(0, wvp_->GetBuffer()->GetGPUVirtualAddress());
	// DrawCall
	commandList->DrawInstanced(lineIndex_, lineIndex_ / kLineVertexCount_, 0, 0);

	// リセット
	Reset();
}

///-------------------------------------------/// 
/// リセット
///-------------------------------------------///
void LineObject3D::Reset() {
	lineIndex_ = 0;
}

///-------------------------------------------/// 
/// 球の形状を計算
///-------------------------------------------///
void LineObject3D::SphereVertexData() {

	const uint32_t kSubdivision = 8; // 分割数
	const float kLonEvery = 2.0f * Math::Pi() / float(kSubdivision); // 経度の1分割の角度
	const float kLatEvery = Math::Pi() / float(kSubdivision); // 緯度の1分割の角度

	// 緯度方向
	for (uint32_t latIndex = 0; latIndex < kSubdivision; latIndex++) {
		float lat = -Math::Pi() / 2.0f + kLatEvery * float(latIndex);
		// 経度方向
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; lonIndex++) {
			float lon = kLonEvery * float(lonIndex);
			// 球の表面上の点を求める
			Vector3 a, b, c;
			a.x = cos(lat) * cos(lon);
			a.y = sin(lat);
			a.z = cos(lat) * sin(lon);

			b.x = cos(lat + kLatEvery) * cos(lon);
			b.y = sin(lat + kLatEvery);
			b.z = cos(lat + kLatEvery) * sin(lon);

			c.x = cos(lat) * cos(lon + kLonEvery);
			c.y = sin(lat);
			c.z = cos(lat) * sin(lon + kLonEvery);

			// 座標を保存
			spheres_.push_back(a);
			spheres_.push_back(b);
			spheres_.push_back(c);
		}
	}
}

///-------------------------------------------/// 
/// Lineの作成
///-------------------------------------------///
void LineObject3D::CreateLine(const Vector3& start, const Vector3& end, const Vector4& color) {
	
	vertexData_[lineIndex_].position = start;
	vertexData_[lineIndex_ + 1].position = end;

	vertexData_[lineIndex_].color = color;
	vertexData_[lineIndex_ + 1].color = color;

	lineIndex_ += kLineVertexCount_;
}