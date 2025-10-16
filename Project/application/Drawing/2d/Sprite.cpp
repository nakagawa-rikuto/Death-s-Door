#include "Sprite.h"
// c++
#include <cassert>
// Engine
#include "Engine/System/Service/GraphicsResourceGetter.h"
#include "Engine/System/Service/Render.h"
// Math
#include "Math/MatrixMath.h"


///-------------------------------------------/// 
/// コンストラクタ、デストラクタ
///-------------------------------------------///
Sprite::~Sprite() = default;


///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// 座標
const Vector2& Sprite::GetPosition() const { return position_; }
// 回転
const float& Sprite::GetRotation() const { return rotation_; }
// サイズ
const Vector2& Sprite::GetSize() const { return size_; }
// 色
const Vector4& Sprite::GetColor() const { return color_; }


///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// 座標
void Sprite::SetPosition(const Vector2& position) { position_ = position; }
// 回転
void Sprite::SetRotation(const float& rotation) { rotation_ = rotation; }
// サイズ
void Sprite::SetSize(const Vector2& size) { size_ = size; }
// 色
void Sprite::SetColor(const Vector4& color) { color_ = color; }
// アンカーポイント
void Sprite::SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }
// フリップ
void Sprite::SetFlipX(const bool& flip) { isFlipX_ = flip; }
void Sprite::SetFlipY(const bool& flip) { isFlipY_ = flip; }
// テクスチャ左上座標
void Sprite::SetTextureLeftTop(const Vector2& textureLeftTop) { textureLeftTop_ = textureLeftTop; }
// テクスチャ切り出しサイズ
void Sprite::SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; }


///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Sprite::Initialize(const std::string textureFilePath) {

	/// ===コマンドリストのポインタの取得=== ///
	ID3D12Device* device = GraphicsResourceGetter::GetDXDevice();

	/// ===テクスチャ=== ///
	filePath_ = textureFilePath;

	/// ===SpriteCommonの初期化=== ///
	SpriteCommon::Initialize(filePath_);

	/// ===WorldTransformの設定=== ///
	worldTransform_ = { {1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f } };
}


///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Sprite::Update() {
	/// ===SpriteCommonの更新=== ///
	SpriteCommon::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void Sprite::Draw(GroundType type, BlendMode mode) {

	/// ===SpriteCommonの描画=== ///
	SpriteCommon::Draw(type, mode);
}


