#include "Model.h"
// c++
#include <cassert>
#include <fstream>
// Engine
#include "Engine/System/Service/GraphicsResourceGetter.h"
#include "Engine/System/Service/Render.h"
// Math
#include "Math/sMath.h"
#include "Math/MatrixMath.h"


///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
Model::Model() = default;
Model::~Model() = default;

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
/// ===モデル=== ///
const Vector3& Model::GetTranslate() const { return worldTransform_.translate; }
const Quaternion& Model::GetRotate() const { return worldTransform_.rotate; }
const Vector3& Model::GetScale() const { return worldTransform_.scale; }
const Vector4& Model::GetColor() const { return color_; }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
/// ===モデル=== ///
void Model::SetTranslate(const Vector3& position) { worldTransform_.translate = position; }
void Model::SetRotate(const Quaternion& rotate) { worldTransform_.rotate = rotate; }
void Model::SetScale(const Vector3& scale) { worldTransform_.scale = scale; }
void Model::SetColor(const Vector4& color) { color_ = color; }
/// ===Light=== ///
void Model::SetLight(LightType type) { ModelCommon::SetLightType(type); }
// LightInfo
void Model::SetLightData(LightInfo light) {light_ = light; }
// 環境マップ
void Model::SetEnviromentMapData(bool flag, float string) { 
	enviromentMapInfo_.isEnviromentMap = flag;
	enviromentMapInfo_.strength = string;
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
// オブジェクトを読み込む場合
void Model::Initialize(const std::string& filename, LightType type) {

	/// ===コマンドリストのポインタの取得=== ///
	ID3D12Device* device = GraphicsResourceGetter::GetDXDevice();

	/// ===モデル読み込み=== ///
	modelData_ = GraphicsResourceGetter::GetModelData(filename); // ファイルパス

	/// ===ModelCommonの初期化=== ///
	ModelCommon::Create(device, type);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Model::Update() {

	/// ===ModelCommonの更新=== ///
	ModelCommon::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void Model::Draw(BlendMode mode) {

	/// ===コマンドリストのポインタの取得=== ///
	ID3D12GraphicsCommandList* commandList = GraphicsResourceGetter::GetDXCommandList();

	/// ===コマンドリストに設定=== ///
	// PSOの設定
	Render::SetPSO(commandList, PipelineType::Obj3D, mode);
	// Viewの設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);
	// ModelCommonの設定
	ModelCommon::Bind(commandList);

	// テクスチャの設定
	Render::SetGraphicsRootDescriptorTable(commandList, 2, modelData_.material.textureFilePath);
	Render::SetGraphicsRootDescriptorTable(commandList, 3, enviromentMapInfo_.textureName);

	// 描画（Drawコール）
	commandList->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
}