#include "LongRangeEnemyBullet.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include "Service/Particle.h"
#include "Service/Collision.h"
#include "Service/DeltaTime.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
LongRangeEnemyBullet::~LongRangeEnemyBullet() {
	Service::Collision::RemoveCollider(this);
	object3d_.reset();
	// パーティクル停止
	if (bulletParticle_) {
		bulletParticle_->Stop();
		bulletParticle_ = nullptr;
	}
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void LongRangeEnemyBullet::Initialize() {
	// Object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(std::make_unique<MiiEngine::Model>(), "Player");
	// Object3dの初期設定
	object3d_->SetTranslate(transform_.translate);

	// Sphereの設定
	SphereCollider::Initialize();
	name_ = MiiEngine::ColliderName::EnemyBullet;
	float radius = 0.5f; // 半径
	SphereCollider::SetRadius(radius);

	// コライダーに追加
	Service::Collision::AddCollider(this);

	object3d_->Update();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void LongRangeEnemyBullet::Update() {
	if (isAlive_) {
		/// ===移動処理=== ///
		Move();

		/// ===タイマー=== ///
		PromoteTimer();

		// 位置更新
		transform_.translate += info_.velocity;
		
		// Particleの位置更新
		if (bulletParticle_) {
			bulletParticle_->SetEmitterPosition(transform_.translate);
		}

		// コライダーの更新
		SphereCollider::Update();
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void LongRangeEnemyBullet::Draw(MiiEngine::BlendMode mode) {mode;}

///-------------------------------------------/// 
/// 生成
///-------------------------------------------///
void LongRangeEnemyBullet::Create(const Vector3& pos, const Vector3& vel, Player* player) {
	// 初期化処理
	Initialize();
	// 位置と方向の設定
	transform_.translate = pos;
	info_.direction = vel;

	// プレイヤーの設定
	player_ = player;

	// 生存フラグをtrue
	isAlive_ = true;
	// 生存時間の設定
	lifeTimer_ = lifeTime_;

	// パーティクル停止
	if (bulletParticle_) {
		bulletParticle_->Stop();
		bulletParticle_ = nullptr;
	}
	// パーティクルの再生
	bulletParticle_ = Service::Particle::Emit("LongEnemyAttack", transform_.translate);
	bulletParticle_->SetEmitterPosition(transform_.translate);
}

///-------------------------------------------///  
/// 衝突判定
///-------------------------------------------///
void LongRangeEnemyBullet::OnCollision(Collider* collider) {
	
	if (collider->GetColliderName() == MiiEngine::ColliderName::Object ||
		collider->GetColliderName() == MiiEngine::ColliderName::Player) {
		// 生存フラグをfalse
		isAlive_ = false;
	}
}

///-------------------------------------------/// 
/// 移動処理
///-------------------------------------------///
void LongRangeEnemyBullet::Move() {
	
	// 向きの更新
	if (player_) {
		// Playerの位置を取得
		Vector3 targetPos = player_->GetTransform().translate;

		// Targetへのベクトルを計算
		Vector3 toPlayer = targetPos - transform_.translate;

		// 徐々にターゲットの方向へ向くように補間
		float trackingSpeed = 0.05f;
		info_.direction = Normalize(info_.direction * (1.0f - trackingSpeed) + Normalize(toPlayer) * trackingSpeed);
	}

	// 速度の更新
	info_.velocity = info_.direction * speed_;
}

///-------------------------------------------/// 
/// タイマーを進める
///-------------------------------------------///
void LongRangeEnemyBullet::PromoteTimer() {
	// ライフタイマーの減少
	lifeTimer_ -= Service::DeltaTime::GetDeltaTime();
	if (lifeTimer_ <= 0.0f) {
		// 生存時間が0.0fになったら削除。
		isAlive_ = false;
	}
}