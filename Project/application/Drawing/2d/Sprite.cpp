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
Sprite::~Sprite() {
	vertex_.reset();
	index_.reset();
	common_.reset();
}


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
	AdjustTextureSize(textureFilePath);

	/// ===生成=== ///
	vertex_ = std::make_unique<VertexBuffer2D>();
	index_ = std::make_unique<IndexBuffer2D>();
	common_ = std::make_unique<SpriteCommon>();

	/// ===vertex=== ///
	// buffer
	vertex_->Create(device, sizeof(VertexData2D) * vertexSize_);
	vertex_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// view
	vertexBufferView_.BufferLocation = vertex_->GetBuffer()->GetGPUVirtualAddress(); // 先頭アドレスから使用
	vertexBufferView_.SizeInBytes = sizeof(VertexData2D) * vertexSize_; // 使用するサイズ（頂点6つ分）
	vertexBufferView_.StrideInBytes = sizeof(VertexData2D); // １頂点当たりのサイズ
	// Data書き込み(初期)
	VertexDataWrite();

	/// ===index=== ///
	// buffer
	index_->Create(device, sizeof(uint32_t) * indexSize_);
	index_->GetBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	// view
	indexBufferView_.BufferLocation = index_->GetBuffer()->GetGPUVirtualAddress(); // 先頭のアドレスから使用
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * indexSize_; // 使用するサイズ（６つ分）
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT; // uint32_tとする
	// Data書き込み(初期)
	IndexDataWrite();

	/// ===マテリアル=== ///
	common_->MaterialInitialize(device, materialSize_);

	/// ===wvp=== ///
	common_->WVPMatrixInitialize(device);

	/// ===WorldTransformの設定=== ///
	worldTransform_ = { {1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f } };
}


///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Sprite::Update() {

	// Data書き込み(更新)
	MateialDataWrite();
	TransformDataWrite();
	UpdateVertexDataWrite();
	SpecifyRange();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void Sprite::Draw(GroundType type, BlendMode mode) {

	/// ===コマンドリストのポインタの取得=== ///
	ID3D12GraphicsCommandList* commandList = GraphicsResourceGetter::GetDXCommandList();

	/// ===コマンドリストに設定=== ///
	// PSOの設定
	if (type == GroundType::Front) {
		Render::SetPSO(commandList, PipelineType::ForGround2D, mode);
	} else if (type == GroundType::Back) {
		Render::SetPSO(commandList, PipelineType::BackGround2D, mode);
	}
	// VertexBufferViewの設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	// IndexBufferViewの設定
	commandList->IASetIndexBuffer(&indexBufferView_);
	// Matrial・WVPの設定
	common_->Bind(commandList);
	// テクスチャの設定
	Render::SetGraphicsRootDescriptorTable(commandList, 2, filePath_);
	// 描画(ドローコール)
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}


///-------------------------------------------/// 
/// VertexResourceの書き込み
///-------------------------------------------///
void Sprite::VertexDataWrite() {
	// 左下
	vertexData_[0].position = { 0.0f, 1.0f, 0.0f, 1.0f };
	vertexData_[0].texcoord = { 0.0f, 1.0f };
	// 左上
	vertexData_[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexData_[1].texcoord = { 0.0f, 0.0f };
	// 右下
	vertexData_[2].position = { 1.0f, 1.0f, 0.0f, 1.0f };
	vertexData_[2].texcoord = { 1.0f, 1.0f };
	// 右上
	vertexData_[3].position = { 1.0f, 0.0f, 0.0f, 1.0f };
	vertexData_[3].texcoord = { 1.0f, 0.0f };
}

///-------------------------------------------/// 
/// IndexResourceの書き込み
///-------------------------------------------///
void Sprite::IndexDataWrite() {
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;
}

///-------------------------------------------/// 
/// MateialDataへの書き込み
///-------------------------------------------///
void Sprite::MateialDataWrite() {
	common_->SetMateiralData(
		color_,
		Math::MakeIdentity4x4()
	);
}


///-------------------------------------------/// 
/// TransformDataの書き込み
///-------------------------------------------///
void Sprite::TransformDataWrite() {

	// 座標の反映
	worldTransform_.translate = { position_.x, position_.y, 0.0f };
	// 回転の反映
	worldTransform_.rotate = { 0.0, 0.0, rotation_ };
	// サイズの反映
	worldTransform_.scale = { size_.x, size_.y, 1.0f };

	// WorldMatrix
	Matrix4x4 worldMatrix = Math::MakeAffineEulerMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	// ViewMatrix
	Matrix4x4 viewMatrix = Math::MakeIdentity4x4();
	// ProjectionMatrix
	Matrix4x4 projectionMatrix = Math::MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(GraphicsResourceGetter::GetWindowWidth()), static_cast<float>(GraphicsResourceGetter::GetWindowHeight()), 0.0f, 100.0f);

	// データの書き込み
	common_->SetWVPData(Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix)));
}


///-------------------------------------------///  
/// UpdateVertexDataWrite
///-------------------------------------------///
void Sprite::UpdateVertexDataWrite() {

	float left = 0.0f - anchorPoint_.x;
	float right = 1.0f - anchorPoint_.x;
	float top = 0.0f - anchorPoint_.y;
	float bottom = 1.0f - anchorPoint_.y;

	// 左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}

	// 上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	// 左下
	vertexData_[0].position = { left, bottom, 0.0f, 1.0f };
	// 左上
	vertexData_[1].position = { left, top, 0.0f, 1.0f };
	// 右下
	vertexData_[2].position = { right, bottom, 0.0f, 1.0f };
	// 右上
	vertexData_[3].position = { right, top, 0.0f, 1.0f };
}


///-------------------------------------------/// 
/// テクスチャ範囲指定
///-------------------------------------------///
void Sprite::SpecifyRange() {
	const DirectX::TexMetadata& metadata = GraphicsResourceGetter::GetMetaData(filePath_);
	float tex_left = textureLeftTop_.x / metadata.width;
	float tex_right = (textureLeftTop_.x + textureSize_.x) / metadata.width;
	float tex_top = textureLeftTop_.y / metadata.height;
	float tex_bottom = (textureLeftTop_.y + textureSize_.y) / metadata.height;

	// 頂点リソースにデータを書き込む
	vertexData_[0].texcoord = { tex_left, tex_bottom };
	vertexData_[1].texcoord = { tex_left, tex_top };
	vertexData_[2].texcoord = { tex_right, tex_bottom };
	vertexData_[3].texcoord = { tex_right, tex_top };
}


///-------------------------------------------/// 
/// テクスチャサイズをイメージに合わせる
///-------------------------------------------///
void Sprite::AdjustTextureSize(const std::string& filePath) {
	// テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = GraphicsResourceGetter::GetMetaData(filePath);

	textureSize_.x = static_cast<float>(metadata.width);
	textureSize_.y = static_cast<float>(metadata.height);

	// 画像サイズをテクスチャサイズに合わせる
	size_ = textureSize_;
}
