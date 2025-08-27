#include "CloseRangeEnemy.h"
// Camera
#include "application/Game/Camera/Camera.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include "Engine/System/Service/InputService.h"
#include "Engine/System/Service/ParticleService.h"
#include "Engine/System/Service/CameraService.h"
#include "Engine/System/Service/ColliderService.h"
// c++
#include <cstdlib>
#include <ctime>
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
CloseRangeEnemy::~CloseRangeEnemy() {
	ColliderService::RemoveCollider(this);
	object3d_.reset();
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void CloseRangeEnemy::SetTranslate(const Vector3& pos) {baseInfo_.translate = pos;}

///-------------------------------------------/// 
/// GameScene用初期化
///-------------------------------------------///
void CloseRangeEnemy::InitGameScene(const Vector3& translate) {
	baseInfo_.translate = translate;
	Initialize();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void CloseRangeEnemy::Initialize() {

	// シードの設定
	//srand(static_cast<unsigned int>(time(nullptr)));

	/// ===MoveInfoの設定=== ///
	moveInfo_.interval = 5.0f;
	moveInfo_.timer = 1.0f;
	moveInfo_.waitTime = 1.5f;
	moveInfo_.range = 10.0f;
	moveInfo_.speed = 0.05f;
	moveInfo_.direction = { 0.0f, 0.0f, 0.0f };
	moveInfo_.rangeCenter = baseInfo_.translate;
	moveInfo_.isWating = false;

	/// ===AttackInfoの設定=== ///
	attackInfo_.range = 4.0f;
	attackInfo_.distance = 15.0f;
	attackInfo_.interval = 6.0f;
	attackInfo_.timer = 0.0f;
	attackInfo_.power = 1;
	attackInfo_.direction = { 0.0f, 0.0f, 0.0f };
	attackInfo_.isAttack = false;

	/// ===ChargeInfo=== ///
	chargeInfo_.moveSpeed = 0.9f;
	chargeInfo_.stopTime = 1.0f;

	/// ===Object=== ///
	// Object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(ObjectType::Model, "player");
	// Object3dの初期設定
	object3d_->SetTranslate(baseInfo_.translate);
	object3d_->SetRotate(baseInfo_.rotate);
	object3d_->SetScale(baseInfo_.scale);
	object3d_->SetColor(baseInfo_.color);

	/// ===SphereCollidr=== ///
	// Sphereの設定
	OBBCollider::Initialize();
	name_ = ColliderName::Enemy;
	obb_.halfSize = { 2.0f, 2.0f, 2.0f };
	// コライダーに追加
	ColliderService::AddCollider(this);

	// BaseEnemyの初期化
	BaseEnemy::Initialize();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void CloseRangeEnemy::Update() {

	// BaseEnemyの更新
	BaseEnemy::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void CloseRangeEnemy::Draw(BlendMode mode) {
	BaseEnemy::Draw(mode);
}

///-------------------------------------------/// 
/// 更新（ImGui）
///-------------------------------------------///
void CloseRangeEnemy::UpdateImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("CloseRangeEnemy");
	BaseEnemy::UpdateImGui();
	ImGui::Text("ChargeInfo");
	ImGui::DragFloat("moveSpeed", &chargeInfo_.moveSpeed, 0.1f);
	ImGui::DragFloat("StopTime", &chargeInfo_.stopTime, 0.1f);
	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 衝突判定
///-------------------------------------------///
void CloseRangeEnemy::OnCollision(Collider* collider) {
	// BaseEnemyの当たり判定
	BaseEnemy::OnCollision(collider);
}

///-------------------------------------------/// 
/// 攻撃処理
///-------------------------------------------///
void CloseRangeEnemy::Attack() {
	// 早期リターン
	if (!player_) return;

	if (!attackInfo_.isAttack) { /// ===IsAttackがfalse=== ///
		// プレイヤー位置を取得
		attackInfo_.playerPos = player_->GetTranslate();

		// プレイヤー位置への方向ベクトル
		Vector3 dir = attackInfo_.playerPos - baseInfo_.translate;
		attackInfo_.direction = Normalize(dir); // 方向を保存

		// directionの方向に回転
		UpdateRotationTowards(attackInfo_.direction, 0.2f);

		// 少し待つ
		if (attackInfo_.timer <= 0.0f) { // タイマーが0以下
			// 移動ベクトルを設定
			baseInfo_.velocity = attackInfo_.direction * chargeInfo_.moveSpeed;

			// 攻撃開始
			attackInfo_.isAttack = true;
		}

	} else { /// ===IsAttackがtrue=== ///

		// プレイヤーとの差を計算
		Vector3 toTarget = attackInfo_.playerPos - baseInfo_.translate;

		// 攻撃終了判定
		if (Length(toTarget) < 0.5f) { // 到達判定s
			attackInfo_.isAttack = false; // 攻撃終了フラグをfalseに
			baseInfo_.velocity = { 0.0f, 0.0f, 0.0f }; // 移動ベクトルをリセット
			attackInfo_.timer = attackInfo_.interval; // クールダウン再設定
			// 移動範囲の中心を設定
			moveInfo_.rangeCenter = attackInfo_.playerPos;

			baseInfo_.color = { 1.0f, 0.0f, 1.0f, 1.0f }; // 元の色に戻す（任意）
		}
	}
}