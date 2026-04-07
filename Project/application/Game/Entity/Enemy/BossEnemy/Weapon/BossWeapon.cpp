#include "BossWeapon.h"
// BossEnemy
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Service
#include "Service/Collision.h"
#include "Service/Particle.h"

///-------------------------------------------/// 
///	デストラクタ
///-------------------------------------------///
BossWeapon::~BossWeapon() {
	object3d_.reset();
}
 
///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void BossWeapon::Initialize() {

	/// ===Object3Dの初期化=== ///
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(std::make_unique<MiiEngine::Model>(), "BossWeapon");

	/// ===OBBCollider=== ///
	OBBCollider::Initialize();
	name_ = MiiEngine::ColliderName::EnemyWeapon;
	SetHalfSize({ 4.0f, 4.0f, 4.0f });

	object3d_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f }); // 初期状態では透明にしておく
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void BossWeapon::Update() {
	if (!isActive_)return;

	// Particleの軌道更新
	if (attackParticle_) {
		attackParticle_->SetEmitterPosition(object3d_->GetWorldTranslate());
	}

	OBBCollider::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void BossWeapon::Draw(MiiEngine::BlendMode mode) {
	if (!isActive_)return;

	OBBCollider::Draw(mode);
}

///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void BossWeapon::Information() {

}

///-------------------------------------------///  
/// 親の設定
///-------------------------------------------///
void BossWeapon::SetUpParent(BossEnemy * parent) {
	bossEnemy_ = parent;
	object3d_->SetParent(bossEnemy_->GetModelCommon());
}

///-------------------------------------------/// 
/// 衝突処理
///-------------------------------------------///
void BossWeapon::OnCollision(MiiEngine::Collider * collider) {
	collider;
}

///-------------------------------------------/// 
/// Seter
///-------------------------------------------///
void BossWeapon::SetActive(bool flag) {
	isActive_ = flag;

	if (isActive_) {
		Service::Collision::AddCollider(this);
		attackParticle_ = Service::Particle::Emit("EnemyAttack", object3d_->GetWorldTranslate());
		attackParticle_->SetEmitterPosition(object3d_->GetWorldTranslate());
		object3d_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	}
	else {
		Service::Collision::RemoveCollider(this);
		if (attackParticle_) {
			// パーティクルの削除
			attackParticle_->Stop();
			attackParticle_ = nullptr;
		}
		object3d_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
	}
}
