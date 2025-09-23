#include "BaseEnemy.h"
// Camera
#include "application/Game/Camera/GameCamera.h"
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
// AttackTimer
float BaseEnemy::GetAttackTimer() const { return attackInfo_.timer; }
// AttackFlag
bool BaseEnemy::GetAttackFlag() const { return attackInfo_.isAttack; }

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
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
	std::seed_seq seq{
		static_cast<uint32_t>(std::time(nullptr)),
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(this)),
	};
	randomEngine_.seed(seq);

	// カメラの取得
	camera_ = CameraService::GetActiveCamera().get();

	// Stateの設定
	ChangeState(std::make_unique<EnemyMoveState>());

	// コライダーに追加
	ColliderService::AddCollider(this);

	// object3dの更新を一回行う
	GameCharacter::Update();
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

	// SphereColliderの更新
	GameCharacter::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void BaseEnemy::Draw(BlendMode mode) {
	GameCharacter::Draw(mode);
}

///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void BaseEnemy::Information() {
#ifdef USE_IMGUI
	// MoveInfo
	ImGui::Text("MoveInfo");
	ImGui::DragFloat("MoveSpeed", &moveInfo_.speed, 0.1f);
	ImGui::DragFloat("MoveRange", &moveInfo_.range, 0.1f);
	ImGui::DragFloat("MoveInterval", &moveInfo_.interval, 0.1f);
	ImGui::DragFloat("MoveWaitTime", &moveInfo_.waitTime, 0.1f);

	// AttackInfo
	ImGui::Text("AttackInfo");
	ImGui::DragFloat("AttackDistance", &attackInfo_.distance, 0.1f);
	ImGui::DragFloat("AttackRange", &attackInfo_.range, 0.1f);
	ImGui::DragFloat("AttackInterval", &attackInfo_.interval, 0.1f);
	ImGui::DragInt("Power", &attackInfo_.power, 1);
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 変更した値をコピー
///-------------------------------------------///
void BaseEnemy::CopyTuningTo(BaseEnemy* enemy) const {
	if (!enemy) return;

	enemy->color_ = color_;

	// ===== Move 系（設計値） ===== //
	enemy->moveInfo_.speed = moveInfo_.speed;
	enemy->moveInfo_.range = moveInfo_.range;
	enemy->moveInfo_.interval = moveInfo_.interval;
	enemy->moveInfo_.waitTime = moveInfo_.waitTime;

	// ===== Attack 系（設計値） ===== //
	enemy->attackInfo_.distance = attackInfo_.distance;
	enemy->attackInfo_.range = attackInfo_.range;
	enemy->attackInfo_.interval = attackInfo_.interval;
	enemy->attackInfo_.power = attackInfo_.power;

	// 型固有の値を派生側でコピー
	this->CopyTypeTuningFromThisTo(enemy);
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
			ParticleService::Emit("Explosion", transform_.translate);
			//ServiceParticle::SetTexture("Cylinder", "gradationLine");
		}
	}
}

///-------------------------------------------/// 
/// 移動処理の開始処理
///-------------------------------------------///
void BaseEnemy::CommonMoveInit() {
	// 速度をリセット
	baseInfo_.velocity = { 0.0f, 0.0f, 0.0f };
}

///-------------------------------------------/// 
/// 移動処理の共通部分
///-------------------------------------------///
void BaseEnemy::CommonMove() {
	// 移動範囲の中心との方向ベクトルを計算（XZ平面）
	Vector3 toCenter = moveInfo_.rangeCenter - transform_.translate;
	// 中心からの距離を取得
	float distanceFromCenter = Length(toCenter);

	/// ===移動処理=== ///
	if (moveInfo_.isWating) { /// ===範囲外に出ていた場合=== ///

		baseInfo_.velocity = { 0.0f, 0.0f, 0.0f }; // 待機中は移動しない

		// 向く方向に回転
		UpdateRotationTowards(moveInfo_.direction, 0.1f);

		if (moveInfo_.timer <= 0.0f) {
			// ランダムな時間を設定
			std::uniform_real_distribution<float> timeDist(1.0f, moveInfo_.interval);
			moveInfo_.timer = timeDist(randomEngine_);

			// 移動ベクトルを設定
			baseInfo_.velocity = moveInfo_.direction * moveInfo_.speed;
			moveInfo_.isWating = false; // 待機フラグを解除
		}

	} else if (distanceFromCenter > moveInfo_.range) { /// ===範囲外に出ていた場合=== ///

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
		target.y = transform_.translate.y;

		// 方向の設定と待機処理の準備
		PreparNextMove(target - transform_.translate);
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
	Quaternion result = Math::SLerp(transform_.rotate, targetRotation, slerpT);
	transform_.rotate = Normalize(result); // ★ 正規化でスケール崩れ防止
}

///-------------------------------------------/// 
/// 時間を進める
///-------------------------------------------///
void BaseEnemy::advanceTimer() {
	// 無敵タイマーを進める
	moveInfo_.timer -= baseInfo_.deltaTIme;

	// 攻撃用のタイマーを進める
	if (attackInfo_.timer > 0.0f) {
		attackInfo_.timer -= baseInfo_.deltaTIme;
	}
}