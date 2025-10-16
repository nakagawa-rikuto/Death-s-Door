#pragma once
/// ===include=== ///
// Engine
#include "Engine/Graphics/2d/Sprite/SpriteCommon.h"

///=====================================================/// 
/// スプライト
///=====================================================///
class Sprite : public SpriteCommon {
public: /// ===基本的な関数=== ///

	Sprite() = default;
	~Sprite();

	// 初期化
	void Initialize(const std::string textureFilePath) override;

	// 更新
	void Update() override;

	// 描画
	void Draw(GroundType type = GroundType::Front, BlendMode mode = BlendMode::KBlendModeNormal) override;

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
};

