#pragma once
/// ===include=== ///
// Engine
#include "Engine/Graphics/2d/SpriteCommon.h"
#include "Engine/Graphics/2d/VertexBuffer2D.h"
#include "Engine/Graphics/2d/IndexBuffer2D.h"
// Pipline
#include "Engine/DataInfo/PipelineStateObjectType.h"

enum class GroundType {
	Front,
	Back
};

///=====================================================/// 
/// スプライト
///=====================================================///
class Sprite {
public: /// ===基本的な関数=== ///

	Sprite() = default;
	~Sprite();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::string textureFilePath);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(GroundType type = GroundType::Front, BlendMode mode = BlendMode::KBlendModeNormal);

public:/// ===Getter=== ///

	// 座標
	const Vector2& GetPosition() const;
	// 回転
	const float& GetRotation() const;
	// サイズ
	const Vector2& GetSize() const;
	// 色
	const Vector4& GetColor() const;

public:/// ===Setter=== ///

	// 座標
	void SetPosition(const Vector2& position);
	// 回転
	void SetRotation(const float& rotation);
	// サイズ
	void SetSize(const Vector2& size);
	// 色
	void SetColor(const Vector4& color);
	// アンカーポイント
	void SetAnchorPoint(const Vector2& anchorPoint);
	// フリップ
	void SetFlipX(const bool& flip);
	void SetFlipY(const bool& flip);
	// テクスチャ左上座標
	void SetTextureLeftTop(const Vector2& textureLeftTop);
	// テクスチャ切り出しサイズ
	void SetTextureSize(const Vector2& textureSize);

private:/// ===Variables(変数)=== ///

	// バッファリソース
	std::unique_ptr<VertexBuffer2D> vertex_;
	std::unique_ptr<IndexBuffer2D> index_;
	std::unique_ptr<SpriteCommon> common_;

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

