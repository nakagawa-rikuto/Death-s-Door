#include "Player.h"
// Camera
#include "application/Game/Camera/GameCamera.h"
// State
#include "State/RootState.h"
// Service
#include "Engine/System/Service/InputService.h"
#include "Engine/System/Service/CameraService.h"
#include "Engine/System/Service/ColliderService.h"
// Math
#include "Math/sMath.h"

// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#include "application/Game/Entity/Enemy/Base/BaseEnemy.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
Player::~Player() {
	object3d_.reset();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// フラグ
bool Player::GetStateFlag(actionType type) const {
	if (type == actionType::kAvoidance) {
		return avoidanceInfo_.isFlag;
	} else if (type == actionType::kCharge) {
		return chargeInfo_.isFlag;
	} else {
		return attackInfo_.isFlag;
	}
}
bool Player::GetpreparationFlag(actionType type) const {
	if (type == actionType::kAvoidance) {
		return avoidanceInfo_.isPreparation;
	} else if (type == actionType::kCharge) {
		return chargeInfo_.isPreparation;
	} else {
		return attackInfo_.isPreparation;
	}
}
// タイマー
float Player::GetTimer(actionType type) {
	if (type == actionType::kAvoidance) {
		return avoidanceInfo_.timer;
	} else if (type == actionType::kCharge) {
		return chargeInfo_.timer;
	} else {
		return attackInfo_.timer;
	}
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// フラグ
void Player::SetStateFlag(actionType type, bool falg) {
	switch (type) {
	case actionType::kAvoidance:
		avoidanceInfo_.isFlag = falg;
		break;
	case actionType::kCharge:
		chargeInfo_.isFlag = falg;
		break;
	case actionType::kAttack:
		attackInfo_.isFlag = falg;
		break;
	}
}
void Player::SetpreparationFlag(actionType type, bool falg) {
	switch (type) {
	case actionType::kAvoidance:
		avoidanceInfo_.isPreparation = falg;
		break;
	case actionType::kCharge:
		chargeInfo_.isPreparation = falg;
		break;
	case actionType::kAttack:
		attackInfo_.isPreparation = falg;
		break;
	}
}
// タイマーの設定
void Player::SetTimer(actionType type, const float& timer) {
	switch (type) {
	case actionType::kAvoidance:
		avoidanceInfo_.timer = timer;
		break;
	case actionType::kCharge:
		chargeInfo_.timer = timer;
		break;
	case actionType::kAttack:
		attackInfo_.timer = timer;
		break;
	}
}
void Player::SetInvicibleTime(const float& time) { 
	invicibleInfo_.timer = invicibleInfo_.time + time; 
}


///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Player::Initialize() {
	// Cameraの設定
	camera_ = CameraService::GetActiveCamera().get();

	// Object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(ObjectType::Model, "player");

	// 初期設定
	ChangState(std::make_unique<RootState>());

	// Sphereの設定
	GameCharacter::Initialize();
	name_ = ColliderName::Player;
	obb_.halfSize = { 1.5f, 1.5f, 1.5f }; 

	// コライダーに追加
	ColliderService::AddCollider(this);
}


///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Player::Update() {
	/// ===スティックの取得=== ///
	StickState rightStick = InputService::GetRightStickState(0);
	camera_->SetStick({ rightStick.x, rightStick.y });

	/// ===タイマーを進める=== ///
	advanceTimer();

	/// ===Stateの管理=== ///
	if (currentState_) {
		// 各Stateの更新
		currentState_->Update(this, camera_);
	}

	/// ===camera=== ///
	camera_->SetTarget(&transform_.translate, &transform_.rotate);

	/// ===SphereColliderの更新=== ///
	GameCharacter::Update();
}


///-------------------------------------------///  
/// 描画
///-------------------------------------------///
void Player::Draw(BlendMode mode) {
	/// ===SphereColliderの描画=== ///
	GameCharacter::Draw(mode);
}


///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void Player::Information() {
#ifdef USE_IMGUI
	ImGui::Begin("Player");
	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat4("Rotate", &transform_.rotate.x, 0.1f);
	ImGui::ColorEdit4("Color", &color_.x);
	ImGui::DragFloat3("Velocity", &baseInfo_.velocity.x, 0.1f);
	ImGui::End();
#endif // USE_IMGUI
}


///-------------------------------------------/// 
/// 衝突
///-------------------------------------------///
void Player::OnCollision(Collider* collider) {

	/// ===GameCharacterの衝突=== ///
	GameCharacter::OnCollision(collider);

	// Colliderによって処理を変更
	if (collider->GetColliderName() == ColliderName::Enemy) {

		if (chargeInfo_.isFlag) {


		} else if (attackInfo_.isFlag) {


		} else {


		}
	}
}

///-------------------------------------------/// 
/// 時間を進める
///-------------------------------------------///
void Player::advanceTimer() {
	// 無敵タイマーを進める
	invicibleInfo_.timer += baseInfo_.deltaTIme;

	// 突進用のタイマーを進める
	if (chargeInfo_.timer > 0.0f) {
		chargeInfo_.timer -= baseInfo_.deltaTIme;
	} else {
		chargeInfo_.isPreparation = true;
	}

	// 回避用タイマーを進める
	if (avoidanceInfo_.timer > 0.0f) {
		avoidanceInfo_.timer -= baseInfo_.deltaTIme;
	} else {
		avoidanceInfo_.isPreparation = true;
	}
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
