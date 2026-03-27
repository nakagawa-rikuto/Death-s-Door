#include "BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include "Service/Particle.h"
// State
#include "State/MoveBossState.h"
#include "State/BossHitReactionState.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI


///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
BossEnemy::~BossEnemy() {
	/// ===状態を解放=== ///
	currentState_->Finalize();
	currentState_.reset();
	/// ===Componentの解放=== ///
	moveComponent_.reset();
	//attackManager_.reset();
	thrustComponent_.reset();
	downswingComponent_.reset();
	jumpSmashComponent_.reset();
	hitReactionComponent_.reset();
	/// ===Object3Dの解放=== ///
	object3d_.reset();
}

///-------------------------------------------/// 
/// ゲームシーンで呼び出す初期化処理
///-------------------------------------------///
void BossEnemy::InitGameScene(const Vector3& translate) {

	/// ===BossEnemyの初期化=== ///
	Initialize();

	/// ===位置の設定=== ///
	transform_.translate = translate;

	/// ===Componentの設定=== ///
	SetComponentConfig();

	// Stateの設定
	ChangeState(std::make_unique<MoveBossState>());
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void BossEnemy::Initialize() {

	/// ===Object3Dの初期化=== ///
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(std::make_unique<MiiEngine::Model>(), "CloseEnemy");

	/// ===BaseEnemyの初期化=== ///
	BaseEnemy::Initialize();

	/// ===OBBのサイズを設定=== ///
	SetHalfSize({ 3.0f, 3.0f, 3.0f });

	/// ===Weapon=== ///
	weapon_ = std::make_unique<BossWeapon>();
	weapon_->Initialize();
	weapon_->SetUpParent(this);
	
	// 無敵時間
	invincibleInfo_.time = 0.5f;

	// HP 
	baseInfo_.HP = 100;
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void BossEnemy::Update() {
	// 死亡している場合は更新しない
	if (isTentativeDeath_) return;

	/// ===Timerの更新=== ///
	advanceTimer();

	/// ===Stateの更新=== ///
	if (currentState_) {
		currentState_->Update();
	}

	/// ==Weaponの更新==== ///
	weapon_->Update();

	/// ===Animationの更新=== ///
	UpdateAnimation();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void BossEnemy::Draw(MiiEngine::BlendMode mode) {

	/// ==Weaponの描画==== ///
	weapon_->Draw(mode);

	/// ===GameCharacterの描画=== ///
	BaseEnemy::Draw(mode);
}

///-------------------------------------------/// 
/// ImGui情報の表示
///-------------------------------------------///
void BossEnemy::Information() {
#ifdef USE_IMGUI
	ImGui::Begin("ボスエネミー");
	/// ===GameCharacterの情報表示=== ///
	GameCharacter::Information();
	/// ===MoveConponentの情報表示=== ///
	moveComponent_->Information();
	/// ===AttackManagerの情報表示=== ///
	thrustComponent_->Information();
	downswingComponent_->Information();
	jumpSmashComponent_->Information();
	//attackManager_->Information();
	/// ===HitReactionComponentの情報表示=== ///
	hitReactionComponent_->Information();
	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 衝突処理
///-------------------------------------------///
void BossEnemy::OnCollision(MiiEngine::Collider* collider) {
	/// ===GameCharacterの衝突=== ///
	BaseEnemy::OnCollision(collider);

	/// ===Playerとの衝突=== ///
	// Weaponとの当たり判定
	if (collider->GetColliderName() == MiiEngine::ColliderName::PlayerWeapon) {
		// クールタイム中でなければノックバック処理を実行
		if (!invincibleInfo_.isInvincible) {
			// 通常攻撃の時
			if (player_->GetAttackComponent()->IsAttacking()) {

				// StateをHitReactionStateに変更
				ChangeState(std::make_unique<BossHitReactionState>());

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
/// 状態の変更
///-------------------------------------------///
void BossEnemy::ChangeState(std::unique_ptr<BossState> nextState) {
	if (currentState_) {
		// 古い状態を解放
		currentState_->Finalize();
		currentState_.reset();
	}

	// 新しい状態をセット
	currentState_ = std::move(nextState);
	// 新しい状態を初期化
	currentState_->Enter(this);
}

///-------------------------------------------/// 
/// パラメータの設定
///-------------------------------------------///
void BossEnemy::SetComponentConfig() {

	attackInfo_.thrustTimer = 0.0f;
	attackInfo_.thrustRange = 6.0f;
	attackInfo_.thrustCooldown = 3.0f;

	attackInfo_.downswingTimer = 0.0f;
	attackInfo_.downswingRange = 8.0f;
	attackInfo_.downswingCooldown = 4.0f;

	attackInfo_.jumpSmashTimer = 0.0f;
	attackInfo_.jumpSmashRange = 14.0f;
	attackInfo_.jumpSmashCooldown = 6.0f;

	/// ===MoveComponentの生成=== ///
	moveComponent_ = std::make_unique<BossMoveComponent>();
	BossMoveComponent::MoveConfig moveConfig{
		.speed = 0.2f,
		.rotationSpeed = 8.0f,
	};
	// 初期化
	moveComponent_->Initialize(moveConfig);

	/// ===HitReactionComponentの生成=== ///
	hitReactionComponent_ = std::make_unique<BossHitReactionComponent>();
	BossHitReactionComponent::KnockBackConfig hitReactionConfig{
		.knockBackForce = 1.5f,
		.slowdownFactor = 0.3f,
		.slowdownDuration = 0.2f,
		.alphaDuration = 0.2f,
		.hitAlpha = 0.6f,
		.flashSpeed = 10.0f,
	};
	// 初期化
	hitReactionComponent_->Initialize(hitReactionConfig);

	/// ===AttackComponentの生成=== ///
	//attackManager_ = std::make_unique<BossAttackManager>();
	thrustComponent_ = std::make_unique<BossAttackThrustComponent>();
	BossAttackThrustComponent::ThrustConfig thrustConfig{
		.windUpAngle = 30.0f,
		.windUpDuration = 0.25f,
		.strikeAngle = -15.0f,
		.strikeDuration = 0.12f,
		.recoveryDuration = 0.5f,
		.weaponRestOffset = { 0.0f,  0.0f,  4.0f },
		.weaponWindUpOffset = { 0.0f,  0.0f, 4.0f },
		.weaponStrikeOffset = { 0.0f,  0.0f, 4.0f },
	};
	// 初期化
	thrustComponent_->Initialize(thrustConfig);

	downswingComponent_ = std::make_unique<BossAttackDownwardSwingComponent>();
	BossAttackDownwardSwingComponent::DownwardSwingConfig downswingConfig{
		.windUpPitch = 15.0f,
		.windUpDuration = 0.35f,
		.strikeForwardPitch = 15.0f,
		.strikeDuration = 0.1f,
		.holdDownDuration = 0.08f,
		.recoveryDuration = 0.4f,
		.strikeStepForward = 0.5f,
		.weaponRestOffset = { 0.0f,  0.5f,  0.3f },
		.weaponWindUpOffset = { 0.0f,  1.4f, -0.4f },
		.weaponStrikeOffset = { 0.0f, -0.2f,  1.5f },
	};
	// 初期化
	downswingComponent_->Initialize(downswingConfig);

	jumpSmashComponent_ = std::make_unique<BossAttackJumpSmashComponent>();
	BossAttackJumpSmashComponent::JumpSmashConfig jumpSmashConfig{
		.leapWindUpCrouchPitch = -10.0f,
		.leapWindUpDuration = 0.3f,
		.leapDuration = 0.6f,
		.leapArcHeight = 4.f,
		.leapAscentPitch = 20.0f,
		.leapDescentPitch = 30.0f,
		.strikeForwardPitch = 25.0f,
		.strikeDuration = 0.1f,
		.holdDownDuration = 0.08f,
		.recoveryDuration = 0.45f,
		.weaponRestOffset = { 0.0f,  0.5f,  0.3f },
		.weaponLeapOffset = { 0.0f,  1.5f, -0.5f },
		.weaponStrikeOffset = { 0.0f, -0.2f,  1.5f },
	};
	// 初期化
	jumpSmashComponent_->Initialize(jumpSmashConfig);

	/*BossAttackManager::Config attackConfig{
		.thrustRange = 4.0f,
		.downswingRange = 6.0f,
		.jumpSmashRange = 14.0f,
		.thrustCooldown = 3.0f,
		.downswingCooldown = 4.0f,
		.jumpSmashCooldown = 6.0f,
		.thrustConfig = thrustConfig,
		.downswingConfig = downswingConfig,
		.jumpSmashConfig = jumpSmashConfig,
	};*/
	// 初期化
	//attackManager_->Initialize(attackConfig);
}


///-------------------------------------------/// 
/// 時間を進める
///-------------------------------------------///
void BossEnemy::advanceTimer() {

	// 攻撃のクールタイマーを進める
	if (attackInfo_.thrustTimer > 0.0f) {
		attackInfo_.thrustTimer -= baseInfo_.deltaTime;
	}
	

	if (baseInfo_.isDead) {
		// パーティクルの発生
		deathParticle_ = Service::Particle::Emit("nakagawa", transform_.translate);
		isTentativeDeath_ = true;
		if (hitParticle_ != nullptr) {
			hitParticle_->Stop();
			hitParticle_ = nullptr;
		}
	} else {
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