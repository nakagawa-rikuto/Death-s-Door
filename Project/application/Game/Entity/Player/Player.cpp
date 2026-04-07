#include "Player.h"
// Camera
#include "Engine/Camera/FollowCamera.h"
// State
#include "State/RootState.h"
#include "State/HitReactionState.h"
// Service
#include "Service/Input.h"
#include "Service/Camera.h"
#include "Service/Collision.h"
#include "Service/Particle.h"
// C++
#include <algorithm>
// Math
#include "Math/sMath.h"
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
Player::~Player() {
	weapon_.reset();
	rightHand_.reset();
	leftHand_.reset();
	moveComponent_.reset();
	avoidanceComponent_.reset();
	attackComponent_.reset();
	currentState_.reset();

	object3d_.reset();
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Cameraの追従対象としてPlayerを設定
void Player::SetCameraTargetPlayer() { camera_->SetTarget(&transform_.translate, &transform_.rotate); }
// 無敵時間の設定
void Player::SetInvincibleTime(const float& time) {
	invincibleInfo_.time = time;
	invincibleInfo_.timer = invincibleInfo_.time;
}

///-------------------------------------------/// 
/// Game開始時の初期化
///-------------------------------------------///
void Player::InitGame(const Vector3& translate, MiiEngine::FollowCamera* camera) {
	/// ===初期化=== ///
	Initialize();

	// Cameraの設定
	camera_ = camera;

	/// ===位置の設定=== ///
	transform_.translate = translate;

	/// ===Handの初期化=== ///
	// 右手
	rightHand_ = std::make_unique<PlayerHand>();
	rightHand_->Initialize();
	rightHand_->SetUpParent(this);
	// 左手
	leftHand_ = std::make_unique<PlayerHand>();
	leftHand_->Initialize();
	leftHand_->SetUpParent(this);

	/// ===Weaponの初期化=== ///
	weapon_ = std::make_unique<PlayerWeapon>();
	weapon_->Initialize();
	weapon_->SetUpParent(this);

	/// ===パラメータの設定=== ///
	SettingParamita();

	/// ===初期Stateの設定=== ///
	ChangState(std::make_unique<RootState>());
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Player::Initialize() {
	// Object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(std::make_unique<MiiEngine::Model>(), "Player");

	// GameCharacterの設定
	GameCharacter::Initialize();
	name_ = MiiEngine::ColliderName::Player;
	// コライダーに追加
	Service::Collision::AddCollider(this);
	// OBBの設定
	SetHalfSize({ 1.25f, 4.0f, 1.0f });
	// 地面との衝突処理のために半サイズYをGroundInfoにセット
	SetHalfSizeY(GetOBB().halfSize.y);

	/// ===AreaInfo=== ///
	areaInfo_.center = { 0.0f, 0.0f, 0.0f };
	areaInfo_.halfSize = { 200.0f, 100.0f, 200.0f };
}


///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Player::Update() {

	// 早期リターン
	if (baseInfo_.isDead) {
		ApplyDeceleration(0.7f);
		UpdateAnimation();
		return;
	}

	/// ===スティックの取得=== ///
	StickState leftStick = Service::Input::GetLeftStickState(0);
	StickState rightStick = Service::Input::GetRightStickState(0);
	// スティック情報を保存
	stickState_ = {
		.leftStick = { leftStick.x, leftStick.y },
		.rightStick = { rightStick.x, rightStick.y }
	};
	// カメラにスティック情報を渡す
	camera_->SetStick(stickState_.rightStick);

	/// ===タイマーを進める=== ///
	advanceTimer();

	/// ===Stateの管理=== ///
	if (currentState_) {
		// 各Stateの更新
		currentState_->Update(this, camera_);
	}

	/// ===更新処理=== ///
	UpdateAnimation();
}

///-------------------------------------------/// 
/// 開始アニメーション時の更新
///-------------------------------------------///
void Player::UpdateAnimation() {

	/// ===Hand=== ///
	rightHand_->Update();
	leftHand_->Update();

	/// ===Weapon=== ///
	weapon_->Update();

	/// ===エリアの衝突処理=== ///
	AreaCollision();

	/// ===GameCharacterの更新=== ///
	GameCharacter::Update();
}

///-------------------------------------------///  
/// 描画
///-------------------------------------------///
void Player::Draw(MiiEngine::BlendMode mode) {

	/// ===Hand=== ///
	rightHand_->Draw(mode);
	leftHand_->Draw(mode);

	/// ===Weapon=== ///
	weapon_->Draw(mode);

	/// ===SphereColliderの描画=== ///
	GameCharacter::Draw(mode);
}


///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void Player::Information() {
#ifdef USE_IMGUI
	ImGui::Begin("Player");
	GameCharacter::Information();		// 親クラスの情報表示
	moveComponent_->Information();		// 移動コンポーネントの情報表示
	avoidanceComponent_->Information(); // 回避コンポーネントの情報表示
	attackComponent_->Information();    // 攻撃コンポーネントの情報表示
	ImGui::DragFloat("無敵時間", &invincibleInfo_.timer, 0.01f);
	weapon_->Information();         // Weaponの情報表示
	ImGui::End();
#endif // USE_IMGUI
}


///-------------------------------------------/// 
/// 衝突
///-------------------------------------------///
void Player::OnCollision(MiiEngine::Collider* collider) {

	/// ===GameCharacterの衝突=== ///
	GameCharacter::OnCollision(collider);

	// Colliderによって処理を変更
	if (collider->GetColliderName() == MiiEngine::ColliderName::EnemyWeapon || collider->GetColliderName() == MiiEngine::ColliderName::EnemyBullet) {

		// 無敵状態でなければダメージを受ける
		if (!invincibleInfo_.isFlag) {

			/// ===カメラのシェイク=== ///
			camera_->StartShake(1.2f, 1.0f);

			// ===ノックバック処理=== ///
			// 敵の位置を取得
			Vector3 enemyPos = collider->GetTransform().translate;
			// プレイヤーから敵への方向ベクトルを計算
			Vector3 knockbackDirection = transform_.translate - enemyPos;
			// ステートの変更
			ChangState(std::make_unique<HitReactionState>(knockbackDirection));
			// 攻撃キャンセル
			attackComponent_->CancelAttack();

			// ダメージ処理
			Service::Particle::Emit("PlayerHitEffect1", transform_.translate);
			baseInfo_.HP--;
			// 無敵状態にする
			SetInvincibleTime(0.5f);
		}
	}
}

///-------------------------------------------/// 
/// Paramitaの設定
///-------------------------------------------///
void Player::SettingParamita() {

	/// ===Component=== ///
	// Componentの生成
	moveComponent_ = std::make_unique<PlayerMoveComponent>();
	avoidanceComponent_ = std::make_unique<PlayerAvoidanceComponent>();
	attackComponent_ = std::make_unique<PlayerAttackComponent>();
	hitReactionComponent_ = std::make_unique<PlayerHitReactionComponent>();

	// MoveComponentの初期化
	PlayerMoveComponent::MoveConfig moveConfig{
		.speed = 0.4f,
		.rotationSpeed = 10.0f,
		.deceleration = 0.85f
	};
	moveComponent_->Initialize(moveConfig);

	// AvoidanceComponentの初期化
	PlayerAvoidanceComponent::AvoidanceConfig avoidanceConfig{
		.speed = 15.0f,
		.activeTime = 0.3f,
		.coolTime = 1.0f,
		.invincibleTime = 0.01f
	};
	avoidanceComponent_->Initialize(avoidanceConfig);

	// HitReactionComponentの初期化
	PlayerHitReactionComponent::HitConfig hitReactionConfig{
		.knockbackSpeed = 1.5f,
	};
	hitReactionComponent_->Initialize(hitReactionConfig);

	// AttackComponentの初期化
	attackComponent_->Initialize();

	// 無敵情報の設定
	invincibleInfo_.isFlag = true; // このフラグをtrueにすると落ちない、falseにするとゲーム開始時から落ちる。

	// HPの設定
	baseInfo_.HP = 8;
}

///-------------------------------------------/// 
/// 時間を進める
///-------------------------------------------///
void Player::advanceTimer() {

	// 無敵タイマーを進める
	if (invincibleInfo_.timer > 0.0f) {
		invincibleInfo_.timer -= baseInfo_.deltaTime;
		invincibleInfo_.isFlag = true;
	} else {
		invincibleInfo_.isFlag = false;
	}

	// 回避タイマーの更新
	avoidanceComponent_->UpdateTimer(baseInfo_.deltaTime);
}

///-------------------------------------------/// 
/// 減速処理
///-------------------------------------------///
void Player::ApplyDeceleration(const float& develeration) {
	// Velocityが0でないなら徐々に0にする
	if (baseInfo_.velocity.x != 0.0f) {
		// 各軸に対して減速適用
		baseInfo_.velocity.x *= develeration;
		// 小さすぎる値は完全に0にスナップ
		if (std::abs(baseInfo_.velocity.x) < 0.01f) {
			baseInfo_.velocity.x = 0.0f;
		}
	}
	if (baseInfo_.velocity.z != 0.0f) {
		baseInfo_.velocity.z *= develeration;
		if (std::abs(baseInfo_.velocity.z) < 0.01f) {
			baseInfo_.velocity.z = 0.0f;
		}
	}
}

///-------------------------------------------/// 
/// Stateの変更
///-------------------------------------------///
void Player::ChangState(std::unique_ptr<PlayerState> newState) {
	if (currentState_) {
		// 古い状態を解放  
		currentState_->Finalize();
		currentState_.reset();
	}

	// 新しい状態をセット  
	currentState_ = std::move(newState);
	// 新しい状態の初期化  
	currentState_->Enter(this, camera_);
}

///-------------------------------------------/// 
/// エリアの衝突処理
///-------------------------------------------///
void Player::AreaCollision() {
	// X軸の制限
	if (areaInfo_.halfSize.x > 0.0f) {
		float minX = areaInfo_.center.x - areaInfo_.halfSize.x;
		float maxX = areaInfo_.center.x + areaInfo_.halfSize.x;
		this->transform_.translate.x = std::clamp(this->transform_.translate.x, minX, maxX);
	}

	// Y軸の制限
	if (areaInfo_.halfSize.y > 0.0f) {
		float minY = areaInfo_.center.y - areaInfo_.halfSize.y;
		float maxY = areaInfo_.center.y + areaInfo_.halfSize.y;
		this->transform_.translate.y = std::clamp(this->transform_.translate.y, minY, maxY);
	}

	// Z軸の制限
	if (areaInfo_.halfSize.z > 0.0f) {
		float minZ = areaInfo_.center.z - areaInfo_.halfSize.z;
		float maxZ = areaInfo_.center.z + areaInfo_.halfSize.z;
		this->transform_.translate.z = std::clamp(this->transform_.translate.z, minZ, maxZ);
	}
}
