#include "BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include "Service/Particle.h"
// State
#include "State/Move/BossMoveState.h"
#include "State/HItReaction/BossHitReactionState.h"
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
	ChangeState(std::make_unique<BossMoveState>());
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
	SetHalfSize({ 7.0f, 7.5f, 7.0f });
	// 地面との衝突処理のために半サイズYをGroundInfoにセット
	SetHalfSizeY(GetOBB().halfSize.y);

	/// ===Weapon=== ///
	weapon_ = std::make_unique<BossWeapon>();
	weapon_->Initialize();
	weapon_->SetUpParent(this);
	
	// 無敵時間
	invincibleInfo_.time = 0.5f;

	// HP 
	baseInfo_.HP = 12;
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
	/// ===MoveComponentの情報表示=== ///
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
				hitParticle_ = Service::Particle::Emit("EnemyHitEffect", transform_.translate);

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
	attackInfo_.thrustRange = 14.0f;
	attackInfo_.thrustCooldown = 2.5f;

	attackInfo_.downswingTimer = 0.0f;
	attackInfo_.downswingRange = 20.0f;
	attackInfo_.downswingCooldown = 4.0f;

	attackInfo_.jumpSmashCooldown = 8.0f;
	attackInfo_.jumpSmashTimer = attackInfo_.jumpSmashCooldown;
	attackInfo_.jumpSmashMinRange = 30.0f;
	attackInfo_.jumpSmashMaxRange = 50.0f;
	

	/// ===MoveComponentの生成=== ///
	moveComponent_ = std::make_unique<BossMoveComponent>();
	BossMoveComponent::MoveConfig moveConfig{
		.speed = 0.2f,
		.rotationSpeed = 8.0f,
	};
	// 初期化
	moveComponent_->Initialize(moveConfig);

	/// ===TeleportComponentの生成=== ///
	teleportComponent_ = std::make_unique<BossTeleportComponent>();
	BossTeleportComponent::TeleportConfig teleportConfig{
		.rotationSpeed = 5.0f,
		.spinOutDuration = 0.6f,
		.warpDuration = 0.4f,
		.spinInDuration = 0.6f,
	};

	/// ===HitReactionComponentの生成=== ///
	hitReactionComponent_ = std::make_unique<BossHitReactionComponent>();
	BossHitReactionComponent::KnockBackConfig hitReactionConfig{
		.knockBackForce = 1.5f,
		.slowdownFactor = 0.5f,
		.slowdownDuration = 0.2f,
		.alphaDuration = 0.2f,
		.hitAlpha = 0.2f,
		.flashSpeed = 10.0f,
	};
	// 初期化
	hitReactionComponent_->Initialize(hitReactionConfig);

	/// ===AttackComponentの生成=== ///
	// Thrust
	thrustComponent_ = std::make_unique<BossAttackThrustComponent>();
	BossAttackThrustComponent::ThrustConfig thrustConfig{
		.windUpAngle = 30.0f,
		.windUpDuration = 0.25f,
		.strikeAngle = -15.0f,
		.strikeDuration = 0.12f,
		.recoveryDuration = 0.5f,
		.weaponRestOffset = { 0.0f,  0.0f,  12.0f },
		.weaponWindUpOffset = { 0.0f,  0.0f, 12.0f },
		.weaponStrikeOffset = { 0.0f,  0.0f, 12.0f },
	};
	// 初期化
	thrustComponent_->Initialize(thrustConfig);

	// DownSwing
	downswingComponent_ = std::make_unique<BossAttackDownwardSwingComponent>();
	BossAttackDownwardSwingComponent::DownwardSwingConfig downswingConfig{
		.windUpPitch = -2.0f,
		.windUpDuration = 1.0f,
		.strikeForwardPitch = -20.0f,
		.strikeDuration = 0.1f,
		.holdDownDuration = 0.2f,
		.recoveryDuration = 0.45f,
		.strikeStepForward = 0.1f,
		.weaponRestOffset = { 0.0f,  0.0f,  12.0f },
	};
	// 初期化
	downswingComponent_->Initialize(downswingConfig);

	// JumpSmash
	jumpSmashComponent_ = std::make_unique<BossAttackJumpSmashComponent>();
	BossAttackJumpSmashComponent::JumpSmashConfig jumpSmashConfig{
		.minDistance = attackInfo_.jumpSmashMinRange,
		.maxDistance = attackInfo_.jumpSmashMaxRange,
		.leapWindUpCrouchPitch = 5.0f,
		.leapWindUpDuration = 0.1f,
		.leapDuration = 0.6f,
		.leapArcHeight = 30.0f,
		.leapAscentPitch = 120.0f,
		.leapDescentPitch = 120.0f,
		.strikeForwardPitch = 20.0f,
		.strikeDuration = 0.2f,
		.holdDownDuration = 0.8f,
		.recoveryDuration = 0.45f,
		.weaponRestOffset = { 0.0f,  0.0f,  12.0f },
	};
	// 初期化
	jumpSmashComponent_->Initialize(jumpSmashConfig);
}


///-------------------------------------------/// 
/// 時間を進める
///-------------------------------------------///
void BossEnemy::advanceTimer() {

	// 攻撃のクールタイマーを進める
	if (attackInfo_.thrustTimer > 0.0f) {
		attackInfo_.thrustTimer -= baseInfo_.deltaTime;
	}
	if (attackInfo_.downswingTimer > 0.0f) {
		attackInfo_.downswingTimer -= baseInfo_.deltaTime;
	}
	if (attackInfo_.jumpSmashTimer > 0.0f) {
		attackInfo_.jumpSmashTimer -= baseInfo_.deltaTime;
	}
	

	if (baseInfo_.isDead) {
		// パーティクルの発生
		deathParticle_ = Service::Particle::Emit("EnemyDeathParticle", transform_.translate);
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