#pragma once
/// ===Include=== ///
// Data
#include "Engine/DataInfo/LineObjectData.h"
#include "Engine/DataInfo/PipelineStateObjectType.h"
// Buffer
#include "Engine/Graphics/3d/Base/VertexBuffer3D.h"
#include "Engine/Graphics/3d/Base/Transform3D.h"
// C++
#include <memory>
#include <vector>

/// ===前方宣言=== ///
class GameCamera;

///=====================================================/// 
/// LineObject3D
///=====================================================///
class LineObject3D {
public:
	LineObject3D() = default;
	~LineObject3D();

	// 初期化
	void Initialize(ID3D12Device* device);
	// 更新
	void Update();
	// 描画
	void Draw();

	// Lineの作成
	void CreateLine(const Vector3& start, const Vector3& end, const Vector4& color);

	// LineIndexのリセット
	void Reset();

public: /// ===Getter=== ///
	const std::vector<Vector3> GetSphereData();

private:

	/// ===バッファリソース=== ///
	std::unique_ptr<VertexBuffer3D> vertex_;
	std::unique_ptr<Transform3D> wvp_;

	/// ===バッファビュー=== ///
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	/// ===バッファリソース内のデータを指すポインタ=== ///
	LineVertexData3D* vertexData_ = nullptr;
	LineTransformMatrixData3D* wvpData_ = nullptr;

	/// ===WorldTransform=== ///
	EulerTransform worldTransform_;
	EulerTransform cameraTransform_;

	/// ===Camera=== ///
	GameCamera* camera_ = nullptr;

	/// ===LineInfo=== ///
	uint32_t lineIndex_ = 0;
	const uint32_t kMaxLineCount_ = 100000;
	const uint32_t kLineVertexCount_ = 2;
	// 
	std::vector<Vector3> spheres_;

private:

	// 
	void SphereVertexData();
};

