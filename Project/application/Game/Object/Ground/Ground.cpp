#include "Ground.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
Ground::~Ground() {
	object3d_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Ground::Initialize() {
	AABBCollider::Initialize();
	//aabb_.min = {};
	//aabb_.max = {};

	// object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(ObjectType::Model, "GameGround");
	object3d_->SetTranslate({ 0.0f, 0.0f, 0.0f });
	object3d_->SetScale({ 0.5f, 0.5f, 0.5f });
	object3d_->Update();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Ground::Update() {
	// 更新
	AABBCollider::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void Ground::Draw(BlendMode mode) {
	// 描画
	AABBCollider::Draw(mode);
}

///-------------------------------------------/// 
/// 衝突判定
///-------------------------------------------///
void Ground::OnCollision(Collider* collider) {}