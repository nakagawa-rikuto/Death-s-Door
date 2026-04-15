#include "BossEnemyBullet.h"
// Service
#include "Service/Particle.h"
#include "Service/Collision.h"
#include "Service/DeltaTime.h"

///-------------------------------------------///
/// デストラクタ
///-------------------------------------------///
BossEnemyBullet::~BossEnemyBullet() {
	object3d_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void BossEnemyBullet::Initialize() {
	// Object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(std::make_unique<MiiEngine::Model>(), "BossEnemyBullet", MiiEngine::LightType::None);

	// SphereColliderの初期化
	SphereCollider::Initialize();
	name_ = MiiEngine::ColliderName::EnemyBullet;
	SphereCollider::SetRadius(0.5f);

	// コライダーに登録
	Service::Collision::AddCollider(this);

	object3d_->Update();
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void BossEnemyBullet::Update() {
	// 生存している場合のみ更新処理を行う
	if (info_.isAlive) {
		/// ===タイマー=== ///
		PromoteTimer();

		/// ===移動処理=== ///
		Move();

		// パーティクルの更新
		if (bulletParticle_) {
			bulletParticle_->SetEmitterPosition(transform_.translate);
		}

		// SphereColliderの更新
		SphereCollider::Update();
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void BossEnemyBullet::Draw(MiiEngine::BlendMode mode) {
	mode;
}

///-------------------------------------------/// 
/// ImGui情報の表示
///-------------------------------------------///
void BossEnemyBullet::Information() {

}

///-------------------------------------------/// 
/// 生成
///-------------------------------------------///
void BossEnemyBullet::Create(const Vector3 & translate, float lifeTim) {
	// 初期化
	Initialize();

	// 位置の設定
	transform_.translate = translate;

	// 生存フラグの設定
	info_.isAlive = true;
	info_.lifeTime = lifeTim;
	lifeTimer_ = info_.lifeTime;

	// パーティクルの停止
	if (bulletParticle_) {
		bulletParticle_->Stop();
		bulletParticle_ = nullptr;
	}
	// パーティクルの再生
	bulletParticle_ = Service::Particle::Emit("BossEnemyAttack", transform_.translate);
	bulletParticle_->SetEmitterPosition(transform_.translate);
}

///-------------------------------------------/// 
/// 衝突処理
///-------------------------------------------///
void BossEnemyBullet::OnCollision(Collider * collider) {
	collider;
}

///-------------------------------------------/// 
/// 移動処理
///-------------------------------------------///
void BossEnemyBullet::Move() {
	// 位置更新
	transform_.translate += info_.velocity;
}

///-------------------------------------------/// 
/// タイマーの更新処理
///-------------------------------------------///
void BossEnemyBullet::PromoteTimer() {
	// ライフタイマーの減少
	lifeTimer_ -= Service::DeltaTime::GetDeltaTime();
	if (lifeTimer_ <= 0.0f) {
		// 生存時間が0.0fになったら削除。
		info_.isAlive = false;
	}
}
