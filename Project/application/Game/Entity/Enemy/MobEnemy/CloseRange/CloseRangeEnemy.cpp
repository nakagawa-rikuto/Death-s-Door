#include "CloseRangeEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include "Service/Particle.h"
// c++
#include <cstdlib>
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
CloseRangeEnemy::~CloseRangeEnemy() {}

///-------------------------------------------/// 
/// GameScene用初期化
///-------------------------------------------///
void CloseRangeEnemy::InitGameScene(const Vector3& translate) {

	/// ===CloseRangeEnemyの初期化=== ///
	Initialize();

	/// ===MobEnemyのInitGameScene=== ///
	MobEnemy::InitGameScene(translate);

	/// ===AttackInfoの設定=== ///
	attackInfo_.range = 4.0f;
	attackInfo_.distance = 15.0f;
	attackInfo_.interval = 6.0f;
	attackInfo_.timer = 0.0f;
	attackInfo_.power = 1;
	attackInfo_.direction = { 0.0f, 0.0f, 0.0f };
	attackInfo_.isAttack = false;

	/// ===ChargeInfo=== ///
	chargeInfo_.moveSpeed = 0.9f;
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void CloseRangeEnemy::Initialize() {

	/// ===Object=== ///
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(std::make_unique<MiiEngine::Model>(), "CloseEnemy");

	/// ===MobEnemyの初期化=== ///
	MobEnemy::Initialize();

	/// ===OBBのサイズを設定=== ///
	SetHalfSize({ 3.0f, 3.0f, 3.0f });
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void CloseRangeEnemy::Update() {
	// MobEnemyの更新
	MobEnemy::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void CloseRangeEnemy::Draw(MiiEngine::BlendMode mode) {
	// MobEnemyの描画
	MobEnemy::Draw(mode);
}

///-------------------------------------------/// 
/// 更新（ImGui）
///-------------------------------------------///
void CloseRangeEnemy::Information() {
#ifdef USE_IMGUI
	ImGui::Begin("近距離攻撃のEnemy");
	MobEnemy::Information();
	ImGui::Text("突進情報");
	ImGui::DragFloat("移動速度", &chargeInfo_.moveSpeed, 0.1f);
	ImGui::DragFloat3("T", &transform_.translate.x);
	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 衝突判定
///-------------------------------------------///
void CloseRangeEnemy::OnCollision(MiiEngine::Collider* collider) {
	// MobEnemyの当たり判定
	MobEnemy::OnCollision(collider);

	/// ===PlayerとObject=== ///
	if (attackInfo_.isAttack) {
		// PlayerとObjectの当たり判定
		if (collider->GetColliderName() == MiiEngine::ColliderName::Player ||
			collider->GetColliderName() == MiiEngine::ColliderName::Object) {
			// 攻撃を解除
			attackInfo_.isAttack = false;
		}
	}
}

///-------------------------------------------/// 
/// 攻撃処理の初期化
///-------------------------------------------///
void CloseRangeEnemy::StartAttack() {
	// 攻撃開始
	attackInfo_.isAttack = true;
	// 攻撃時間の設定
	attackInfo_.timer = 1.0f;
	// 移動ベクトルを設定
	baseInfo_.velocity = attackInfo_.direction * chargeInfo_.moveSpeed;
	// パーティクル停止
	if (attackParticle_) {
		attackParticle_->Stop();
		attackParticle_ = nullptr;
	}
	// パーティクルの再生
	attackParticle_ = Service::Particle::Emit("CloseEnemyAttack", transform_.translate);
	attackParticle_->SetEmitterPosition(transform_.translate);
}

///-------------------------------------------/// 
/// 攻撃処理
///-------------------------------------------///
void CloseRangeEnemy::Attack() {

	// プレイヤーとの差を計算
	Vector3 toTarget = attackInfo_.targetPos - transform_.translate;
	float length = Length(toTarget);

	// パーティクルの位置更新
	if (attackParticle_) {
		attackParticle_->SetEmitterPosition(transform_.translate);
	}

	// 攻撃終了判定
	if (length < 0.5f) { // 到達判定

		// 攻撃終了フラグをfalse
		attackInfo_.isAttack = false;
		// 移動ベクトルをリセット
		baseInfo_.velocity = { 0.0f, 0.0f, 0.0f };
		// クールダウン再設定
		attackInfo_.timer = attackInfo_.interval;
	}
}