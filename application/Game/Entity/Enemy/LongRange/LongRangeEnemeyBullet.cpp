#include "LongRangeEnemeyBullet.h"
#include "Engine/System/Service/ColliderService.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
LongRangeEnemeyBullet::~LongRangeEnemeyBullet() {
	ColliderService::RemoveCollider(this);
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
bool LongRangeEnemeyBullet::GetIsAlive() {
	return isAlive_;
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void LongRangeEnemeyBullet::Initialize() {
	// Object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(ObjectType::Model, "player");
	// Object3dの初期設定
	object3d_->SetTranslate(transform_.translate);

	// Sphereの設定
	SphereCollider::Initialize();
	name_ = ColliderName::Enemy;
	sphere_.radius = 0.5f;

	// コライダーに追加
	ColliderService::AddCollider(this);

	object3d_->Update();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void LongRangeEnemeyBullet::Update() {
	if (isAlive_) {
		/// ===移動処理=== ///
		Move();

		/// ===タイマー=== ///
		PromoteTimer();

		transform_.translate += info_.velocity;

		// コライダーの更新
		SphereCollider::Update();
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void LongRangeEnemeyBullet::Draw(BlendMode mode) {
	if (isAlive_) {
		SphereCollider::Draw(mode);
	}
}

///-------------------------------------------/// 
/// 生成
///-------------------------------------------///
void LongRangeEnemeyBullet::Create(const Vector3& pos, const Vector3& vel) {
	transform_.translate = pos;
	info_.direction = vel;

	// 生存フラグをtrue
	isAlive_ = true;
	// 生存時間の設定
	lifeTimer_ = lifeTime_;

	Initialize();
}

///-------------------------------------------///  
/// 衝突判定
///-------------------------------------------///
void LongRangeEnemeyBullet::OnCollision(Collider * collider) {
	
}

///-------------------------------------------/// 
/// 移動処理
///-------------------------------------------///
void LongRangeEnemeyBullet::Move() {
	info_.velocity = info_.direction * speed_;
}

///-------------------------------------------/// 
/// タイマーを進める
///-------------------------------------------///
void LongRangeEnemeyBullet::PromoteTimer() {
	// ライフタイマーの減少
	lifeTimer_ -= 1.0f / 60.0f;
	if (lifeTimer_ <= 0.0f) {
		// 生存時間が0.0fになったら削除。
		isAlive_ = false; 
	}
}