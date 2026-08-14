#include "ModelCommon.h"
// Math
#include "Math/MatrixMath.h"
#include "Math/sMath.h"
// Service
#include "Service/Render.h"
#include "Service/Camera.h"
// Camera
#include "Engine/Camera/Base/CameraCommon.h"

namespace MiiEngine {
	///-------------------------------------------/// 
	/// コンストラクタ・デストラクタ
	///-------------------------------------------///
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
	void ModelCommon::ClearParent() {
		SetParent(nullptr);
		parentOffset_ = { 0.0f,0.0f,0.0f };
	}
	// Offset
	void ModelCommon::SetParentOffset(const Vector3& offset) { parentOffset_ = offset; }
	const Vector3& ModelCommon::GetParentOffset() const { return parentOffset_; }

	///-------------------------------------------/// 
	/// Joint
	///-------------------------------------------///
	// ジョイントのワールド行列を取得
	Matrix4x4 ModelCommon::GetJointWorldMatrix(const std::string& jointName) const {
		jointName;
		Matrix4x4 worldMatrix = Math::MakeIdentity4x4();
		return worldMatrix;
	}
	// ジョイントの親を設定
	void ModelCommon::SetJointParent(ModelCommon* parent, const std::string& jointName) {
		jointParentModel_ = parent;
		jointParentName_ = jointName;
	}
	// ジョイントの親を解除
	void ModelCommon::ClearJointParent() { jointParentModel_ = nullptr; }

	///-------------------------------------------/// 
	/// Setter
	///-------------------------------------------///
	// Transform
	void ModelCommon::SetTranslate(const Vector3& position) {
		worldTransform_.translate = position;
		worldTransformCacheDirtyTag_.translate = true;
	}
	void ModelCommon::SetRotate(const Quaternion& rotate) {
		worldTransform_.rotate = rotate;
		worldTransformCacheDirtyTag_.rotate = true;
	}
	void ModelCommon::SetScale(const Vector3& scale) {
		worldTransform_.scale = scale;
		worldTransformCacheDirtyTag_.scale = true;
	}
	// Color
	void ModelCommon::SetColor(const Vector4& color) { color_ = color; }
	// Light
	void ModelCommon::SetLightType(LightType type) { common_->SetLightType(type); }
	void ModelCommon::SetLightData(LightInfo light) { light_ = light; }
	// 環境マップ
	void ModelCommon::SetEnvironmentMapData(bool flag, float string) {
		environmentMapInfo_.isEnvironmentMap = flag;
		environmentMapInfo_.strength = string;
	}

	///-------------------------------------------/// 
	/// Getter
	///-------------------------------------------///
	// Translate 
	const Vector3& ModelCommon::GetWorldTranslate() const {
		// キャッシュが有効ならそのまま返す
		if (!worldTransformCacheDirtyTag_.translate) {
			return cachedWorldTransform_.translate;
		}

		/// ===ワールド行列から位置を取得=== ///
		cachedWorldTransform_.translate = {
			worldMatrix_.m[3][0],
			worldMatrix_.m[3][1],
			worldMatrix_.m[3][2]
		};

		worldTransformCacheDirtyTag_.translate = false; // キャッシュを有効にする
		return cachedWorldTransform_.translate;
	}
	// Rotate
	const Quaternion& ModelCommon::GetWorldRotate() const {
		// キャッシュが有効ならそのまま返す
		if (!worldTransformCacheDirtyTag_.rotate) {
			return cachedWorldTransform_.rotate;
		}
		/// ===ワールド行列から回転を取得=== ///
		Matrix4x4 rotateMatrix = {}; // 回転行列
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				rotateMatrix.m[i][j] = worldMatrix_.m[i][j];
			}
		}
		// スケールで割って正規化
		if (cachedWorldTransform_.scale.x != 0.0f) {
			rotateMatrix.m[0][0] /= cachedWorldTransform_.scale.x;
			rotateMatrix.m[0][1] /= cachedWorldTransform_.scale.x;
			rotateMatrix.m[0][2] /= cachedWorldTransform_.scale.x;
		}
		if (cachedWorldTransform_.scale.y != 0.0f) {
			rotateMatrix.m[1][0] /= cachedWorldTransform_.scale.y;
			rotateMatrix.m[1][1] /= cachedWorldTransform_.scale.y;
			rotateMatrix.m[1][2] /= cachedWorldTransform_.scale.y;
		}
		if (cachedWorldTransform_.scale.z != 0.0f) {
			rotateMatrix.m[2][0] /= cachedWorldTransform_.scale.z;
			rotateMatrix.m[2][1] /= cachedWorldTransform_.scale.z;
			rotateMatrix.m[2][2] /= cachedWorldTransform_.scale.z;
		}
		// 回転行列からクォータニオンに変換
		cachedWorldTransform_.rotate = Math::MatrixToQuaternion(rotateMatrix);

		/// ===ワールド行列から位置を取得=== ///
		cachedWorldTransform_.translate = {
			worldMatrix_.m[3][0],
			worldMatrix_.m[3][1],
			worldMatrix_.m[3][2]
		};

		worldTransformCacheDirtyTag_.rotate = false; // キャッシュを有効にする
		return cachedWorldTransform_.rotate;
	}
	// Scale
	const Vector3& ModelCommon::GetWorldScale() const {
		// キャッシュが有効ならそのまま返す
		if (!worldTransformCacheDirtyTag_.scale) {
			return cachedWorldTransform_.scale;
		}

		// ===ワールド行列からスケールを取得=== ///
		cachedWorldTransform_.scale.x = std::sqrt(
			worldMatrix_.m[0][0] * worldMatrix_.m[0][0] +
			worldMatrix_.m[0][1] * worldMatrix_.m[0][1] +
			worldMatrix_.m[0][2] * worldMatrix_.m[0][2]
		);
		cachedWorldTransform_.scale.y = std::sqrt(
			worldMatrix_.m[1][0] * worldMatrix_.m[1][0] +
			worldMatrix_.m[1][1] * worldMatrix_.m[1][1] +
			worldMatrix_.m[1][2] * worldMatrix_.m[1][2]
		);
		cachedWorldTransform_.scale.z = std::sqrt(
			worldMatrix_.m[2][0] * worldMatrix_.m[2][0] +
			worldMatrix_.m[2][1] * worldMatrix_.m[2][1] +
			worldMatrix_.m[2][2] * worldMatrix_.m[2][2]
		);

		worldTransformCacheDirtyTag_.scale = false; // キャッシュを有効にする
		return cachedWorldTransform_.scale;
	}
	// Transform（位置、回転、拡縮）を取得
	const QuaternionTransform& ModelCommon::GetWorldTransform() const {

		cachedWorldTransform_.scale = GetWorldScale();
		cachedWorldTransform_.rotate = GetWorldRotate();
		cachedWorldTransform_.translate = GetWorldTranslate();

		return cachedWorldTransform_;
	}
	// Color
	const Vector4& ModelCommon::GetColor() const { return color_; }
	// WorldMatrix
	const Matrix4x4& ModelCommon::GetWorldMatrix() const { return worldMatrix_; }

	///-------------------------------------------/// 
	/// 初期化 
	///-------------------------------------------///
	void ModelCommon::Initialize(const std::string& modelName, LightType type) {
		modelName;
		type;
	}

	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void ModelCommon::Update() {
		/// ===カメラの設定=== ///
		camera_ = Service::Camera::GetActiveCamera();

		// MaterialDataの書き込み
		MaterialDataWrite();
		// Transform情報の書き込み
		TransformDataWrite();
		// Lightの書き込み
		LightDataWrite();
		// Cameraの書き込み
		CameraDataWrite();
		// 環境マップの書き込み
		EnvironmentMapDataWrite();
	}

	///-------------------------------------------/// 
	/// 描画 
	///-------------------------------------------///
	void ModelCommon::Draw(BlendMode mode) {
		mode;
	}

	///-------------------------------------------/// 
	/// 作成
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
		environmentMapInfo_ = {
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
	/// 描画
	///-------------------------------------------///
	void ModelCommon::Bind(ID3D12GraphicsCommandList* commandList) {

		/// ===コマンドリストに設定=== ///
		// Commonの設定
		common_->Bind(commandList);

		// テクスチャの設定
		Service::Render::SetGraphicsRootDescriptorTable(commandList, 2, modelData_.material.textureFilePath);
		Service::Render::SetGraphicsRootDescriptorTable(commandList, 3, environmentMapInfo_.textureName);
	}

	///-------------------------------------------/// 
	/// MaterialDataの書き込み
	///-------------------------------------------///
	void ModelCommon::MaterialDataWrite() {
		/// ===Matrixの作成=== ///
		Matrix4x4 uvTransformMatrix = Math::MakeScaleMatrix(uvTransform_.scale);
		Matrix4x4 uvTransformMatrixMultiply = Multiply(uvTransformMatrix, Math::MakeRotateZMatrix(uvTransform_.rotate.z));
		uvTransformMatrixMultiply = Multiply(uvTransformMatrixMultiply, Math::MakeTranslateMatrix(uvTransform_.translate));

		/// ===値の代入=== ///
		common_->SetMaterialData(
			color_,
			light_.shininess,
			uvTransformMatrixMultiply
		);
	}

	///-------------------------------------------/// 
	/// Transform情報の書き込み
	///-------------------------------------------///
	void ModelCommon::TransformDataWrite() {
		// ワールド行列の作成
		worldMatrix_ = Math::MakeAffineQuaternionMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
		Matrix4x4 worldViewProjectionMatrix;

		/// ===親の確認=== ///
		if (jointParentModel_) {
			// ボーンのワールド行列を親として合成
			Matrix4x4 jointWorldMatrix = jointParentModel_->GetJointWorldMatrix(jointParentName_);
			worldMatrix_ = Multiply(worldMatrix_, jointWorldMatrix);
		} else if (parent_) {
			// 親のワールド行列
			Matrix4x4 parentWorldMatrix = Math::MakeAffineQuaternionMatrix(parent_->worldTransform_.scale, parent_->worldTransform_.rotate, parent_->worldTransform_.translate);
			// オフセット行列を作成（親の回転と拡大縮小を適用するため）
			Matrix4x4 offsetMatrix = Math::MakeTranslateMatrix(parentOffset_);
			// 合成
			worldMatrix_ = Multiply(worldMatrix_, offsetMatrix);
			worldMatrix_ = Multiply(worldMatrix_, parentWorldMatrix);
		}

		// キャッシュを無効化
		worldTransformCacheDirtyTag_.translate = true;
		worldTransformCacheDirtyTag_.rotate = true;
		worldTransformCacheDirtyTag_.scale = true;

		/// ===Matrixの作成=== ///
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix_, viewProjectionMatrix);

		/// ===値の代入=== ///
		common_->SetTransformData(
			worldViewProjectionMatrix,
			Multiply(modelData_.rootNode.localMatrix, worldMatrix_),
			Math::Inverse4x4(worldMatrix_)
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
	void ModelCommon::EnvironmentMapDataWrite() {
		common_->SetEnvironmentMapData(environmentMapInfo_.isEnvironmentMap, environmentMapInfo_.strength);
	}
}