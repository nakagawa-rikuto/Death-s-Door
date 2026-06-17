#include "Player.h"
// AttackDataSerializer
#include "Engine/System/Editor/Attack/AttackDataSerializer.h"
// Camera
#include "Engine/Camera/FollowCamera.h"
// State
#include "State/RootState.h"
#include "State/HitReactionState.h"
#include "State/DeathState.h"
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
	// weapon、hand、stateの解放
	weapon_.reset();
	rightHand_.reset();
	leftHand_.reset();
	currentState_.reset();
	// Object3dの解放
	object3d_.reset();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// 攻撃データをIDで取得できるようにする
const AttackData* Player::GetAttackData(int attackID) const {
	auto it = attackDataMap_.find(attackID);
	if (it != attackDataMap_.end()) {
		return &it->second;
	}
	return nullptr;
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

	/// ===LightInfo=== ///
	SetLight(MiiEngine::LightType::PointLight);
	baseInfo_.lightInfo_.shininess = 30.0f;
	baseInfo_.lightInfo_.point = {
		.color = { 1.0f, 1.0f, 1.0f, 1.0f },
		.position = { transform_.translate.x, 100.0f, transform_.translate.z },
		.intensity = 10.0f,
		.radius = 115.0f,
		.decay = 1.5f,
	};
	SetLightData(baseInfo_.lightInfo_);

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

	/// ===AttackDataの読み込み=== ///
	LoadAttackData(0, "Resource/Json/Attacks/PlayerAttack_0.json");
	LoadAttackData(1, "Resource/Json/Attacks/PlayerAttack_1.json");
	LoadAttackData(2, "Resource/Json/Attacks/PlayerAttack_2.json");
}


///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Player::Update() {

	// 死亡時の処理
	if (baseInfo_.isDead) {
		// currentState_がDeathStateでない場合、DeathStateに変更する
		if (dynamic_cast<DeathState*>(currentState_.get()) == nullptr) {
			ChangState(std::make_unique<DeathState>());
		}
	}

	/// ===スティックの取得=== ///
	StickState leftStick = Service::Input::GetLeftStickState(0);
	// スティック情報を保存
	leftStickValue_ = { leftStick.x, leftStick.y };

	/// ===タイマーを進める=== ///
	advanceTimer();

	// ライトの位置をキャラクターの位置に合わせる
	baseInfo_.lightInfo_.point.position.x = transform_.translate.x;
	baseInfo_.lightInfo_.point.position.z = transform_.translate.z;
	SetLightData(baseInfo_.lightInfo_);

	/// ===Stateの管理=== ///
	if (currentState_) {
		// 各Stateの更新
		currentState_->Update();
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

	/// ===パラメーターの調整=== ///
	// MoveComponentのパラメーター設定
	parameters_.move.speed = 0.4f;
	parameters_.move.rotationSpeed = 10.0f;
	parameters_.move.deceleration = 0.85f;
	// DodgeComponentのパラメーター設定
	parameters_.dodge.speed = 20.0f;
	parameters_.dodge.activeTime = 0.3f;
	parameters_.dodge.coolTime = 1.0f;
	parameters_.dodge.invincibleTime = 0.01f;
	// HitReactionComponentのパラメーター設定
	parameters_.hitReaction.knockbackSpeed = 1.5f;

	// 無敵情報の設定
	invincibleInfo_.isFlag = true; // このフラグをtrueにすると落ちない、falseにするとゲーム開始時から落ちる。

	// HPの設定
	baseInfo_.HP = 8;
}

///-------------------------------------------/// 
/// 攻撃データの読み込み
///-------------------------------------------///
void Player::LoadAttackData(int attackID, const std::string& filePath) {
	AttackData data;
	if (AttackDataSerializer::LoadFromJson(data, filePath)) {
		data.attackID = attackID; // 読み込んだデータに攻撃IDを設定
		attackDataMap_[attackID] = data;
	}
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
	if (coolDownInfo_.timer > 0.0f) {
		coolDownInfo_.timer -= baseInfo_.deltaTime;
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
