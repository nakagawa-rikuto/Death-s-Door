#pragma once
/// ===include=== ///
// Engine
#include "Engine/Graphics/2d/Base/Object2DCommon.h"
#include "Engine/Graphics/2d/Base/VertexBuffer2D.h"
#include "Engine/Graphics/2d/Base/IndexBuffer2D.h"
// Pipline
#include "Engine/DataInfo/PipelineStateObjectType.h"

enum class GroundType {
	Front,
	Back
};

///=====================================================/// 
/// スプライト共通部
///=====================================================///
class SpriteCommon {
public: /// ===基本的な関数=== ///

	SpriteCommon() = default;
	~SpriteCommon();

	// 初期化
	virtual void Initialize(const std::string textureFilePath);

	// 更新
	virtual void Update();

	// 描画
	virtual void Draw(GroundType type, BlendMode mode);

private:/// ===Variables(変数)=== ///

	// バッファリソース
	std::unique_ptr<VertexBuffer2D> vertex_;
	std::unique_ptr<IndexBuffer2D> index_;
	std::unique_ptr<Object2DCommon> common_;

	// サイズ
	uint32_t vertexSize_ = 6;
	uint32_t indexSize_ = 6;
	uint32_t materialSize_ = 3;

	// バッファリソース内のデータを指すポインタ
	VertexData2D* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;

	// バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

protected:

	// WorldTransform
	EulerTransform worldTransform_; // Transform(scale, rotate, transform)

	/// ===スプライト情報=== ///
	std::string filePath_;                       // テクスチャファイルパス
	Vector2 position_ = { 0.0f, 0.0f };          // 座標
	float rotation_ = 0.0f;                      // 回転
	Vector2 size_ = { 640.0f, 360.0f };          // サイズ
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f }; // 色
	Vector2 anchorPoint_ = { 0.0f, 0.0f };       // アンカーポイント
	bool isFlipX_ = false;                       // 左右フリップ
	bool isFlipY_ = false;                       // 上下フリップ
	Vector2 textureLeftTop_ = { 0.0f, 0.0f };    // テクスチャ左上
	Vector2 textureSize_ = { 100.0f, 100.0f };   // テクスチャ切り出しサイズ

private:/// ===Functions(関数)=== ///

	// VertexResourceへの書き込み
	void VertexDataWrite();
	// IndexResourceへの書き込み
	void IndexDataWrite();
	// MateialDataへの書き込み
	void MateialDataWrite();
	// Transform情報の書き込み
	void TransformDataWrite();
	// UpdateVertexDataWrite
	void UpdateVertexDataWrite();
	// テクスチャ範囲指定
	void SpecifyRange();
	// テクスチャサイズをイメージに合わせる
	void AdjustTextureSize(const std::string& filePath);
};

