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

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="textureFilePath">初期化に使用するテクスチャファイルのパスを表す文字列。</param>
	void Initialize(const std::string textureFilePath) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="type">描画するグラウンドの種類。既定値は GroundType::Front。</param>
	/// <param name="mode">使用するブレンドモード。既定値は BlendMode::KBlendModeNormal。</param>
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

