#include "BossEnemy.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include "Service/Particle.h"
// State
#include "State/Move/BossMoveState.h"
#include "State/HItReaction/BossHitReactionState.h"
// ImGui
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
	attackComponentManager_.reset();
	hitReactionComponent_.reset();
	/// ==Weapon==== ///
	weapon_.reset();
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

	/// ===LightInfo=== ///
	SetLight(MiiEngine::LightType::PointLight);
	baseInfo_.lightInfo_.shininess = 30.0f;
	baseInfo_.lightInfo_.point = {
		.color = { 1.0f, 1.0f, 1.0f, 1.0f },
		.position = { transform_.translate.x, 80.0f, transform_.translate.z - 50.0f},
		.intensity = 10.0f,
		.radius = 115.0f,
		.decay = 1.5f,
	};
	SetLightData(baseInfo_.lightInfo_);

	/// ===BulletManagerの初期化=== ///
	bulletManager_ = std::make_unique<BossBulletManager>();
	bulletManager_->Initialize();

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
	if (baseInfo_.isDead) {
		DeathUpdate();
		return;
	}

	/// ===Timerの更新=== ///
	advanceTimer();

	// ライトの位置をキャラクターの位置に合わせる
	baseInfo_.lightInfo_.point.position.x = transform_.translate.x;
	baseInfo_.lightInfo_.point.position.z = transform_.translate.z - 30.0f;

	/// ===AttackComponentManagerの更新=== ///
	attackComponentManager_->Update(transform_.translate, baseInfo_.deltaTime);

	/// ===Stateの更新=== ///
	if (currentState_) {
		currentState_->Update();
	}

	/// ===BulletManagerの更新=== ///
	bulletManager_->Update();

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
	// LightInfoの情報表示
	ImGui::DragFloat("光沢度", &baseInfo_.lightInfo_.shininess, 0.01f);
	if (ImGui::TreeNode("Point")) {
		ImGui::ColorEdit3("ライトの色", &baseInfo_.lightInfo_.point.color.x);
		ImGui::DragFloat3("ライトの位置", &baseInfo_.lightInfo_.point.position.x, 0.01f);
		ImGui::DragFloat("ライトの強さ", &baseInfo_.lightInfo_.point.intensity, 0.01f);
		ImGui::DragFloat("ライトの半径", &baseInfo_.lightInfo_.point.radius, 0.01f);
		ImGui::DragFloat("ライトの減衰率", &baseInfo_.lightInfo_.point.decay, 0.01f);
		ImGui::TreePop();
	}
	SetLightData(baseInfo_.lightInfo_);
	/// ===GameCharacterの情報表示=== ///
	GameCharacter::Information();
	/// ===MoveComponentの情報表示=== ///
	moveComponent_->Information();
	/// ===AttackComponentManagerの情報表示=== ///
	attackComponentManager_->Information();
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
	// 初期化
	teleportComponent_->Initialize(teleportConfig);

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

	/// ===AttackComponentManagerの生成=== ///
	attackComponentManager_ = std::make_unique<BossAttackComponentManager>();
	BossAttackComponentManager::Config attackConfig{};
	// Rotate
	attackConfig.rotateRange = 14.0f;
	attackConfig.rotateCooldown = 2.5f;
	// DownSwing
	attackConfig.downswingRange = 20.0f;
	attackConfig.downswingCooldown = 4.0f;
	// JumpSmash
	attackConfig.jumpSmashCooldown = 8.0f;
	attackConfig.jumpSmashMinRange = 30.0f;
	attackConfig.jumpSmashMaxRange = 50.0f;
	// OrbitingOrbs
	attackConfig.orbitingOrbsRange = 40.0f;
	attackConfig.orbitingOrbsCooldown = 6.0f;
	// ParabolicShot
	attackConfig.parabolicShotRange = 35.0f;
	attackConfig.parabolicShotCooldown = 5.0f;

	/// ===AttackComponentの生成=== ///
	// Rotate
	attackConfig.rotateConfig = BossAttackRotateComponent::RotateConfig{
		.windUpDuration = 3.0f,
		.strikeAngle = 30.0f,
		.strikeDuration = 0.3f,
		.recoveryDuration = 0.5f,
		.weaponOffset = { 0.0f,  0.0f,  12.0f },
	};

	// DownSwing
	attackConfig.downswingConfig = BossAttackDownwardSwingComponent::DownwardSwingConfig{
		.windUpPitch = -2.0f,
		.windUpDuration = 1.0f,
		.strikeForwardPitch = -20.0f,
		.strikeDuration = 0.1f,
		.holdDownDuration = 0.2f,
		.recoveryDuration = 0.45f,
		.strikeStepForward = 0.1f,
		.weaponRestOffset = { 0.0f,  0.0f,  12.0f },
	};

	// JumpSmash
	attackConfig.jumpSmashConfig = BossAttackJumpSmashComponent::JumpSmashConfig{
		.minDistance = attackConfig.jumpSmashMinRange,
		.maxDistance = attackConfig.jumpSmashMaxRange,
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

	// OrbitingOrbs
	attackConfig.orbitingOrbsConfig = BossAttackOrbitingOrbsComponent::OrbitConfig{
		.orbitRadius = 3.0f,
		.orbitSpeedDeg = 120.0f,
		.orbitHeight = 0.0f,
		.lifetime = 5.0f,
		.initialAngleDeg = 120.0f,
	};

	// ParabolicShot
	attackConfig.parabolicShotConfig = BossAttackParabolicShotComponent::ParabolicConfig{
		.launchAngleDeg = 80.0f,
		.gravity = 4.9f,
		.lifetime = 5.0f,
		.trembleDuration = 3.0f,
		.enableGroundHit = true,
		.maxHorizontalSpeed = 0.6f,
	};

	// AttackComponentManagerの初期化
	attackComponentManager_->Initialize(attackConfig, bulletManager_.get());
}


///-------------------------------------------/// 
/// 時間を進める
///-------------------------------------------///
void BossEnemy::advanceTimer() {

	// 死んでなければ
	if (!baseInfo_.isDead) {
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

///-------------------------------------------/// 
///	死亡時の処理
///-------------------------------------------///
void BossEnemy::DeathUpdate() {

	if (!isTentativeDeath_) {
		// ヒットパーティクルを止める
		if (hitParticle_) {
			hitParticle_->Stop();
			hitParticle_ = nullptr;
		}
		// パーティクルの発生
		deathParticle_ = Service::Particle::Emit("EnemyDeathParticle", transform_.translate);

		// フラグを立てる
		isTentativeDeath_ = true;
		
		// 速度を0にする
		baseInfo_.velocity = {0.0f, 0.0f, 0.0f};
		// 透明度を下げる
		color_ = { 1.0f, 1.0f, 1.0f, 0.0f };
		// コライダーを消す
		SetColliderActive(false);
	}
}
