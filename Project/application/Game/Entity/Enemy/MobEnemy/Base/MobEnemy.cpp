#include "MobEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include "application/Game/Entity/Enemy/MobEnemy/State/EnemyHitReactionState.h"
#include "application/Game/Entity/Enemy/MobEnemy/State/EnemyMoveState.h"
// Service
#include "Service/Particle.h"
// Math
#include "Math/sMath.h"
#include "Math/EasingMath.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
MobEnemy::~MobEnemy() {
	// Particleの解放
	if (spawnParticle_ != nullptr) {
		spawnParticle_->Stop();
		spawnParticle_ = nullptr;
	}
	// 状態を解放
	currentState_->Finalize();
	currentState_.reset();
	// Componentの解放
	moveComponent_.reset();
	teleportComponent_.reset();
	hitReactionComponent_.reset();
}

///-------------------------------------------/// 
/// ゲームシーンの初期化
///-------------------------------------------///
void MobEnemy::InitGameScene(const Vector3& translate) {
	/// ===位置の設定=== ///
	transform_.translate = translate;

	/// ===パラメータ設定=== ///
	SettingParamita();

	/// ===Particleの発生=== ///
	spawnParticle_ = Service::Particle::Emit("MobEnemySpawn", transform_.translate);

	/// ===Stateの設定=== ///
	ChangeState(std::make_unique<EnemyMoveState>());
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void MobEnemy::Initialize() {
	/// ===BaseEnemyの初期化=== ///
	BaseEnemy::Initialize();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void MobEnemy::Update() {
	// 死亡している場合は更新しない
	if (isTentativeDeath_) return;

	/// ===Timerの更新=== ///
	advanceTimer();

	/// ===Stateの更新=== ///
	if (currentState_ && !baseInfo_.isDead) {
		// 各Stateの更新
		currentState_->Update();
	}

	/// ===BaseEnemyの更新=== ///
	UpdateAnimation();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void MobEnemy::Draw(MiiEngine::BlendMode mode) {
	/// ===BaseEnemyの描画=== ///
	BaseEnemy::Draw(mode);
}

///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void MobEnemy::Information() {
#ifdef USE_IMGUI
	// MoveComponent
	moveComponent_->Information();

	// AttackInfo
	ImGui::Text("攻撃情報");
	ImGui::DragFloat("攻撃可能距離", &attackInfo_.distance, 0.1f);
	ImGui::DragFloat("攻撃範囲", &attackInfo_.range, 0.1f);
	ImGui::DragFloat("攻撃間隔", &attackInfo_.interval, 0.1f);
	ImGui::DragInt("パワー", &attackInfo_.power, 1);

	// HitReactionComponent
	hitReactionComponent_->Information();

	// InvincibleInfo
	ImGui::Separator();
	ImGui::Text("無敵情報");
	ImGui::DragFloat("無敵時間", &invincibleInfo_.time, 0.01f, 0.0f, 5.0f);
	ImGui::Text("無敵タイマー: %.2f", invincibleInfo_.timer);
	ImGui::Text("無敵中: %s", invincibleInfo_.isInvincible ? "true" : "false");

#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 当たり判定
///-------------------------------------------///
void MobEnemy::OnCollision(Collider* collider) {

	/// ===BaseEnemy=== ///
	BaseEnemy::OnCollision(collider);

	// Weaponとの当たり判定
	if (collider->GetColliderName() == MiiEngine::ColliderName::PlayerWeapon) {
		// クールタイム中でなければノックバック処理を実行
		if (!invincibleInfo_.isInvincible) {
			// 通常攻撃の時
			if (player_->GetAttackComponent()->IsAttacking()) {
				// StateをHitReaction変更
				ChangeState(std::make_unique<EnemyHitReactionState>());

				// ノックバック方向の計算
				Vector3 toWeapon = collider->GetTransform().translate - transform_.translate;
				toWeapon.y = 0.0f; // Y軸は無視
				toWeapon = -Normalize(toWeapon); // 反転して正規化
				hitReactionComponent_->OnHit(toWeapon);

				// HPを減少
				baseInfo_.HP--;
				hitParticle_ = Service::Particle::Emit("Game", transform_.translate);

				// 無敵時間のセット
				SetInvincibleTime();
			}
		}
	}
}

///-------------------------------------------/// 
/// Stateの変更
///-------------------------------------------///
void MobEnemy::ChangeState(std::unique_ptr<EnemyState> newState) {
	if (currentState_) {
		// 古い状態を解放
		currentState_->Finalize();
		currentState_.reset();
	}

	// 新しい状態をセット
	currentState_ = std::move(newState);
	// 新しい状態を初期化
	currentState_->Enter(this);
}

///-------------------------------------------/// 
/// 攻撃可能かチェック
///-------------------------------------------///
bool MobEnemy::CheckAttackable() {

	// 敵の前方向ベクトル(Y軸回転を使用)
	float yaw = transform_.rotate.y;
	Vector3 forward = {
		std::sinf(yaw),
		0.0f,
		std::cosf(yaw)
	};

	Vector3 toPlayer = player_->GetTransform().translate - transform_.translate;;
	toPlayer.y = 0.0f;
	float distance = Length(toPlayer);

	// 距離判定
	if (distance <= attackInfo_.distance) {
		Vector3 dirToPlayer = Normalize(toPlayer);
		float dot = Dot(forward, dirToPlayer);
		float angleToPlayer = std::acos(dot); // ラジアン

		// 角度が範囲内かチェック
		if (angleToPlayer <= attackInfo_.range) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

///-------------------------------------------/// 
/// パラメータ設定
///-------------------------------------------///
void MobEnemy::SettingParamita() {
	/// ===Componentの生成=== ///
	moveComponent_ = std::make_unique<EnemyMoveComponent>();
	teleportComponent_ = std::make_unique<EnemyTeleportComponent>();
	hitReactionComponent_ = std::make_unique<EnemyHitReactionComponent>();
	// MoveComponentの初期化
	EnemyMoveComponent::MoveConfig moveConfig{
			.speed = 0.05f,
			.rotationSpeed = 0.1f,
			.chaseRange = 16.0f,
			.evadeRange = 2.0f,
			.moveTime = 2.0f,
			.evadeTime = 4.0f
	};
	moveComponent_->Initialize(moveConfig);
	// TeleportComponentの初期化
	EnemyTeleportComponent::TeleportConfig teleportConfig{
		.rotationSpeed = 5.0f,
		.spinOutDuration = 0.5f,
		.warpDuration = 0.3f,
		.spinInDuration = 0.5f
	};
	teleportComponent_->Initialize(teleportConfig);
	// HitReactionComponentの初期化
	EnemyHitReactionComponent::KnockBackConfig hitReactionConfig{
		.knockBackForce = 1.5f,
		.slowdownFactor = 0.5f,
		.slowdownDuration = 0.2f,
		.alphaDuration = 0.2f,
		.hitAlpha = 0.1f,
		.flashSpeed = 10.0f
	};
	hitReactionComponent_->Initialize(hitReactionConfig);

	// 無敵情報の初期化
	invincibleInfo_.time = 0.5f;
	invincibleInfo_.timer = 0.0f;
	invincibleInfo_.isInvincible = false;

	// 消滅タイマー
	disappearTimer_ = 3.0f;
	isTentativeDeath_ = false;

	// HP 
	baseInfo_.HP = 2;
}

///-------------------------------------------/// 
/// 時間を進める
///-------------------------------------------///
void MobEnemy::advanceTimer() {

	if (baseInfo_.isDead) {
		// 消えるまでの時間を進める
		disappearTimer_ -= baseInfo_.deltaTime;
		if (disappearTimer_ <= 0) {
			deathParticle_ = Service::Particle::Emit("nakagawa", transform_.translate);
			isTentativeDeath_ = true;
			if (hitParticle_ != nullptr) {
				hitParticle_->Stop();
				hitParticle_ = nullptr;
			}
		}
	} else {
		// 攻撃用のタイマーを進める
		if (attackInfo_.timer > 0.0f) {
			attackInfo_.timer -= baseInfo_.deltaTime;
		}

		// 無敵タイマーを進める
		if (invincibleInfo_.timer > 0.0f) {
			invincibleInfo_.timer -= baseInfo_.deltaTime;
			invincibleInfo_.isInvincible = true;
		} else {
			invincibleInfo_.isInvincible = false;
			invincibleInfo_.timer = 0.0f;
		}
	}
}