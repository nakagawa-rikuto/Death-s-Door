#include "BaseEnemy.h"
// Camera
#include "application/Game/Camera/Camera.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// State
#include "application/Game/Entity/Enemy/State/EnemyMoveState.h"
// Service
#include "Engine/System/Service/InputService.h"
#include "Engine/System/Service/ParticleService.h"
#include "Engine/System/Service/CameraService.h"
#include "Engine/System/Service/ColliderService.h"
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
BaseEnemy::~BaseEnemy() {
	object3d_.reset();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// Transform
Vector3 BaseEnemy::GetTranslate() const { return baseInfo_.translate; }
Quaternion BaseEnemy::GetRotate() const { return baseInfo_.rotate; }
// AttackTimer
float BaseEnemy::GetAttackTimer() const { return attackInfo_.timer; }
// AttackFlag
bool BaseEnemy::GetAttackFlag() const { return attackInfo_.isAttack; }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Transform
void BaseEnemy::SetTranslate(const Vector3& translate) { baseInfo_.translate = translate; }
void BaseEnemy::SetRotate(const Quaternion& rotate) { baseInfo_.rotate = rotate; }
// 移動量 
void BaseEnemy::SetVelocity(const Vector3& vel) { baseInfo_.velocity = vel; }
// 色
void BaseEnemy::SetColor(const Vector4& color) { baseInfo_.color = color; }
// タイマー
void BaseEnemy::SetTimer(StateType type, float time) {
	switch (type) {
	case StateType::Move:
		moveInfo_.timer = time;
		break;
	case StateType::Attack:
		attackInfo_.timer = time;
		break;
	}
}
void BaseEnemy::SetPlayer(Player* player) { player_ = player; }

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void BaseEnemy::Initialize() {

	// ランダムエンジンの初期化
	randomEngine_.seed(static_cast<unsigned int>(std::time(nullptr)));

	// カメラの取得
	camera_ = CameraService::GetActiveCamera().get();

	// Stateの設定
	ChangeState(std::make_unique<EnemyMoveState>());

	// コライダーに追加
	ColliderService::AddCollider(this);

	// object3dの更新を一回行う
	object3d_->Update();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void BaseEnemy::Update() {

	/// ===Timerの更新=== ///
	advanceTimer();

	/// ===Stateの更新=== ///
	if (currentState_) {
		// 各Stateの更新
		currentState_->Update(this);
	}

	/// ===移動量の反映=== ///
	baseInfo_.translate += baseInfo_.velocity;

	/// ===Object3dの更新=== ///
	object3d_->SetTranslate(baseInfo_.translate);
	object3d_->SetRotate(baseInfo_.rotate);
	object3d_->SetColor(baseInfo_.color);

	// SphereColliderの更新
	OBBCollider::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void BaseEnemy::Draw(BlendMode mode) {
	OBBCollider::Draw(mode);
}

///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void BaseEnemy::UpdateImGui() {
#ifdef USE_IMGUI
	// Base情報
	ImGui::Text("BaseInfo");
	ImGui::DragFloat3("translate", &baseInfo_.translate.x, 0.1f);
	ImGui::DragFloat4("rotate", &baseInfo_.rotate.x, 0.1f);
	ImGui::DragFloat3("velocity", &baseInfo_.velocity.x, 0.1f);

	// MoveInfo
	ImGui::Text("MoveInfo");
	ImGui::DragFloat("MoveTimer", &moveInfo_.timer, 0.1f);
	ImGui::DragFloat("MoveSpeed", &moveInfo_.speed, 0.1f);
	ImGui::DragFloat("MoveRange", &moveInfo_.range, 0.1f);
	ImGui::DragFloat("MoveInterval", &moveInfo_.interval, 0.1f);
	ImGui::DragFloat("MoveWaitTime", &moveInfo_.waitTime, 0.1f);
	ImGui::DragFloat3("MoveCenter", &moveInfo_.rangeCenter.x, 0.1f);
	ImGui::DragFloat3("MoveDirection", &moveInfo_.direction.x, 0.1f);
	ImGui::Checkbox("IsWating", &moveInfo_.isWating);

	// AttackInfo
	ImGui::Text("AttackInfo");
	ImGui::DragFloat("AttackTimer", &attackInfo_.timer, 0.1f);
	ImGui::DragFloat("AttackDistance", &attackInfo_.distance, 0.1f);
	ImGui::DragFloat("AttackRange", &attackInfo_.range, 0.1f);
	ImGui::DragFloat("AttackInterval", &attackInfo_.interval, 0.1f);
	ImGui::DragInt("Power", &attackInfo_.power, 1);
	ImGui::DragFloat3("AttackDirection", &attackInfo_.direction.x, 0.1f);
	ImGui::DragFloat3("PlayerPos", &attackInfo_.playerPos.x, 0.1f);
	ImGui::Checkbox("IsAttack", &attackInfo_.isAttack);
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 当たり判定
///-------------------------------------------///
void BaseEnemy::OnCollision(Collider* collider) {
	// Playerとの当たり判定
	if (collider->GetColliderName() == ColliderName::Player) {
		// Plaeyrの突進に対しての衝突判定
		if (player_->GetStateFlag(actionType::kCharge)) {
			// パーティクルを発生
			ParticleService::Emit("Explosion", baseInfo_.translate);
			//ServiceParticle::SetTexture("Cylinder", "gradationLine");
		}
	}
}

///-------------------------------------------/// 
/// 移動処理の開始処理
///-------------------------------------------///
void BaseEnemy::CommonMoveInit() {
	// 移動範囲の中心を設定
	moveInfo_.rangeCenter = attackInfo_.playerPos;
	// 速度をリセット
	baseInfo_.velocity = { 0.0f, 0.0f, 0.0f };
}

///-------------------------------------------/// 
/// 移動処理の共通部分
///-------------------------------------------///
void BaseEnemy::CommonMove() {
	// 移動範囲の中心との方向ベクトルを計算（XZ平面）
	Vector3 toCenter = moveInfo_.rangeCenter - baseInfo_.translate;

	// 中心からの距離を取得
	float distanceFromCenter = Length(toCenter);

	/// ===移動処理=== ///
	if (moveInfo_.isWating) { /// ===範囲外に出ていた場合=== ///

		baseInfo_.velocity = { 0.0f, 0.0f, 0.0f }; // 待機中は移動しない

		// 向く方向に回転
		UpdateRotationTowards(moveInfo_.direction, 0.2f);

		if (moveInfo_.timer <= 0.0f) {
			// ランダムな時間を設定
			std::uniform_real_distribution<float> timeDist(1.0f, moveInfo_.interval);
			moveInfo_.timer = timeDist(randomEngine_);

			// 移動ベクトルを設定
			baseInfo_.velocity = moveInfo_.direction * moveInfo_.speed;
			moveInfo_.isWating = false; // 待機フラグを解除
		}

	} else if (distanceFromCenter > moveInfo_.range) { /// ===待機中=== ///

		// 方向の設定と待機処理の準備
		PreparNextMove(toCenter);

	} else if (moveInfo_.timer <= 0.0f && !moveInfo_.isWating) { /// ===範囲内かつタイマーが切れていた場合=== ///

		// ランダムな角度と距離を生成
		std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * Math::Pi());
		std::uniform_real_distribution<float> distanceDist(0.0f, moveInfo_.range);
		// ランダムな値の設定
		float angle = angleDist(randomEngine_);
		float distance = distanceDist(randomEngine_);

		// 方向ベクトルを円から算出
		Vector3 offset = {
			std::cos(angle) * distance,
			0.0f,
			std::sin(angle) * distance
		};

		// 移動先の座標を計算
		Vector3 target = moveInfo_.rangeCenter + offset;
		target.y = baseInfo_.translate.y;

		// 方向の設定と待機処理の準備
		PreparNextMove(target - baseInfo_.translate);
	}
}

///-------------------------------------------/// 
/// // 方向の設定と待機時間の設定
///-------------------------------------------///
void BaseEnemy::PreparNextMove(const Vector3& vector) {
	Vector3 dir = Normalize(vector);
	dir.y = 0.0f; // Y成分を0にしてXZ平面での方向ベクトルを作成
	moveInfo_.direction = Normalize(dir); // 方向を保存

	// 待機時間を設定
	moveInfo_.timer = moveInfo_.waitTime; // 待機時間を設定
	// 待機フラグをtrueに設定
	moveInfo_.isWating = true;
}

///-------------------------------------------/// 
/// 攻撃可能かチェック
///-------------------------------------------///
bool BaseEnemy::CheckAttackable() {

	// 敵の前方向ベクトル（Y軸回転を使用）
	float yaw = baseInfo_.rotate.y;
	Vector3 forward = {
		std::sinf(yaw),
		0.0f,
		std::cosf(yaw)
	};

	Vector3 toPlayer = player_->GetTranslate() - baseInfo_.translate;;
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
/// Stateの変更
///-------------------------------------------///
void BaseEnemy::ChangeState(std::unique_ptr<EnemyState> newState) {
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
/// 回転更新関数
///-------------------------------------------///
void BaseEnemy::UpdateRotationTowards(const Vector3& direction, float slerpT) {
	if (Length(direction) < 0.001f) return;

	// forward方向からターゲットクォータニオンを作成
	Quaternion targetRotation = Math::LookRotation(direction, Vector3(0.0f, 1.0f, 0.0f));

	// SLerp補間
	Quaternion result = Math::SLerp(baseInfo_.rotate, targetRotation, slerpT);
	baseInfo_.rotate = Normalize(result); // ★ 正規化でスケール崩れ防止
}

///-------------------------------------------/// 
/// 時間を進める
///-------------------------------------------///
void BaseEnemy::advanceTimer() {
	// 無敵タイマーを進める
	moveInfo_.timer -= deltaTime_;

	// 攻撃用のタイマーを進める
	if (attackInfo_.timer > 0.0f) {
		attackInfo_.timer -= deltaTime_;
	}
}