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
	bulletManager_->Initialize(parameters_.orbitingAttack);

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

	/// ===Stateの更新=== ///
	if (currentState_) {
		currentState_->Update();
	}

	/// ===BulletManagerの更新=== ///
	bulletManager_->Update(baseInfo_.deltaTime);

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
	
	/// ===Componentの情報表示=== ///
	BossComponent::Information(parameters_);

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
				
				// ノックバック方向の計算
				Vector3 toWeapon = collider->GetTransform().translate - transform_.translate;
				toWeapon.y = 0.0f; // Y軸は無視
				toWeapon = -Normalize(toWeapon); // 反転して正規化
				// StateをHitReactionStateに変更
				ChangeState(std::make_unique<BossHitReactionState>(toWeapon));

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

	/// ===Parametersの設定=== ///
	parameters_.attackRange.rotateAttack = 14.0f;
	parameters_.attackRange.downwardSwingAttack = 20.0f;
	parameters_.attackRange.jumpSmashMin = 30.0f;
	parameters_.attackRange.jumpSmashMax = 50.0f;
	parameters_.attackRange.orbitingOrbs = 40.0f;
	parameters_.attackRange.parabolicShot = 35.0f;

	parameters_.attackCooldown.rotateAttack = 2.5f;
	parameters_.attackCooldown.downwardSwingAttack = 4.0f;
	parameters_.attackCooldown.jumpSmashAttack = 8.0f;
	parameters_.attackCooldown.orbitingOrbs = 6.0f;
	parameters_.attackCooldown.parabolicShot = 5.0f;
	

	parameters_.move.moveSpeed = 0.2f;
	parameters_.move.rotationSpeed = 8.0f;

	parameters_.teleport.rotationSpeed = 5.0f;
	parameters_.teleport.spinOutDuration = 0.6f;
	parameters_.teleport.warpDuration = 0.4f;
	parameters_.teleport.spinInDuration = 0.6f;

	parameters_.hitReaction.knockBackForce = 1.5f;
	parameters_.hitReaction.slowdownFactor = 0.5f;
	parameters_.hitReaction.slowdownDuration = 0.2f;
	parameters_.hitReaction.alphaDuration = 0.2f;
	parameters_.hitReaction.hitAlpha = 0.2f;
	parameters_.hitReaction.flashSpeed = 10.0f;

	parameters_.rotateAttack.windUpDuration = 1.6f;
	parameters_.rotateAttack.strikeAngle = 30.0f;
	parameters_.rotateAttack.strikeDuration = 0.3f;
	parameters_.rotateAttack.recoveryDuration = 0.5f;
	parameters_.rotateAttack.weaponOffset = { 0.0f,  0.0f,  12.0f };

	parameters_.downwardSwingAttack.windUpPitch = -2.0f;
	parameters_.downwardSwingAttack.windUpDuration = 1.0f;
	parameters_.downwardSwingAttack.strikeForwardPitch = -20.0f;
	parameters_.downwardSwingAttack.strikeDuration = 0.1f;
	parameters_.downwardSwingAttack.holdDownDuration = 0.2f;
	parameters_.downwardSwingAttack.recoveryDuration = 0.45f;
	parameters_.downwardSwingAttack.strikeStepForward = 0.1f;
	parameters_.downwardSwingAttack.weaponOffset = { 0.0f,  0.0f,  12.0f };

	parameters_.jumpSmashAttack.minDistance = parameters_.attackRange.jumpSmashMin;
	parameters_.jumpSmashAttack.maxDistance = parameters_.attackRange.jumpSmashMax;
	parameters_.jumpSmashAttack.leapWindUpCrouchPitch = 5.0f;
	parameters_.jumpSmashAttack.leapWindUpDuration = 0.1f;
	parameters_.jumpSmashAttack.leapDuration = 0.6f;
	parameters_.jumpSmashAttack.leapArcHeight = 30.0f;
	parameters_.jumpSmashAttack.leapAscentPitch = 120.0f;
	parameters_.jumpSmashAttack.leapDescentPitch = 120.0f;
	parameters_.jumpSmashAttack.strikeForwardPitch = 20.0f;
	parameters_.jumpSmashAttack.strikeDuration = 0.2f;
	parameters_.jumpSmashAttack.holdDownDuration = 0.8f;
	parameters_.jumpSmashAttack.recoveryDuration = 0.45f;
	parameters_.jumpSmashAttack.weaponOffset = { 0.0f,  0.0f,  12.0f };

	parameters_.orbitingAttack.bulletCount = 3;
	parameters_.orbitingAttack.orbitRadius = 3.0f;
	parameters_.orbitingAttack.orbitSpeed = 120.0f;
	parameters_.orbitingAttack.orbitHeight = 0.0f;
	parameters_.orbitingAttack.lifeTime = 5.0f;
	parameters_.orbitingAttack.initialAngle = 120.0f;

	parameters_.parabolicShotAttack.launchAngle = 80.0f;
	parameters_.parabolicShotAttack.gravity = 4.9f;
	parameters_.parabolicShotAttack.lifeTime = 5.0f;
	parameters_.parabolicShotAttack.trebleDuration = 3.0f;
	parameters_.parabolicShotAttack.rotationSpeed = 8.0f;
	parameters_.parabolicShotAttack.enableGroundHit = true;
	parameters_.parabolicShotAttack.maxHorizontalSpeed = 0.6f;
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

	// 攻撃のクールダウンを進める
	cooldownTimer_.rotateAttack= (std::max)(0.0f, cooldownTimer_.rotateAttack - GetDeltaTime());
	cooldownTimer_.downwardSwingAttack = (std::max)(0.0f, cooldownTimer_.downwardSwingAttack - GetDeltaTime());
	cooldownTimer_.jumpSmashAttack = (std::max)(0.0f, cooldownTimer_.jumpSmashAttack - GetDeltaTime());
	cooldownTimer_.orbitingOrbs = (std::max)(0.0f, cooldownTimer_.orbitingOrbs - GetDeltaTime());
	cooldownTimer_.parabolicShot = (std::max)(0.0f, cooldownTimer_.parabolicShot - GetDeltaTime());
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
