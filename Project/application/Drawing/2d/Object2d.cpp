#include "Object2d.h"
// c++
#include <cassert>
// Engine
#include "Service/GraphicsResourceGetter.h"
#include "Service/Render.h"
// Math
#include "Math/MatrixMath.h"

///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
Object2d::~Object2d() = default;

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// 座標
const Vector2& Object2d::GetPosition() const { return position_; }
// 回転
const float& Object2d::GetRotation() const { return rotation_; }
// サイズ
const Vector2& Object2d::GetSize() const { return size_; }
// 色
const Vector4& Object2d::GetColor() const { return color_; }


///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// 座標
void Object2d::SetPosition(const Vector2& position) { position_ = position; }
// 回転
void Object2d::SetRotation(const float& rotation) { rotation_ = rotation; }
// サイズ
void Object2d::SetSize(const Vector2& size) { size_ = size; }
// 色
void Object2d::SetColor(const Vector4& color) { color_ = color; }
// アンカーポイント
void Object2d::SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }
// フリップ
void Object2d::SetFlipX(const bool& flip) { isFlipX_ = flip; }
void Object2d::SetFlipY(const bool& flip) { isFlipY_ = flip; }
// テクスチャ左上座標
void Object2d::SetTextureLeftTop(const Vector2& textureLeftTop) { textureLeftTop_ = textureLeftTop; }
// テクスチャ切り出しサイズ
void Object2d::SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; }


///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Object2d::Initialize(const std::string textureFilePath, MiiEngine::GroundType type) {

	/// ===テクスチャ=== ///
	filePath_ = textureFilePath;

	/// ===SpriteCommonの初期化=== ///
	MiiEngine::SpriteCommon::Initialize(filePath_, type);

	/// ===WorldTransformの設定=== ///
	worldTransform_ = { {1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f } };
}

