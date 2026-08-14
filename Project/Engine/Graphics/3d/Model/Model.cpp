#include "Model.h"
// c++
#include <cassert>
#include <fstream>
// Engine
#include "Service/GraphicsResourceGetter.h"
#include "Service/Render.h"
// Math
#include "Math/sMath.h"
#include "Math/MatrixMath.h"

namespace MiiEngine {
	///-------------------------------------------/// 
	/// コンストラクタ、デストラクタ
	///-------------------------------------------///
	Model::Model() = default;
	Model::~Model() = default;

	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	// オブジェクトを読み込む場合
	void Model::Initialize(const std::string& filename, LightType type) {

		/// ===コマンドリストのポインタの取得=== ///
		ID3D12Device* device = Service::GraphicsResourceGetter::GetDXDevice();

		/// ===モデル読み込み=== ///
		modelData_ = Service::GraphicsResourceGetter::GetModelData(filename); // ファイルパス

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
		ID3D12GraphicsCommandList* commandList = Service::GraphicsResourceGetter::GetDXCommandList();

		/// ===コマンドリストに設定=== ///
		// PSOの設定
		Service::Render::SetPSO(commandList, PipelineType::Obj3D, mode);
		// Viewの設定
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
		commandList->IASetIndexBuffer(&indexBufferView_);
		// ModelCommonの設定
		ModelCommon::Bind(commandList);

		// 描画（Drawコール）
		commandList->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
	}

	///-------------------------------------------/// 
	/// ジョイントのワールド行列を取得
	///-------------------------------------------///
	Matrix4x4 Model::GetJointWorldMatrix(const std::string& jointName) const { return ModelCommon::GetJointWorldMatrix(jointName); }
}