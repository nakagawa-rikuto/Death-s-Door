#include "LongRangeEnemy.h"
// Camera
#include "application/Game/Camera/Camera.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include "Engine/System/Service/InputService.h"
#include "Engine/System/Service/ParticleService.h"
#include "Engine/System/Service/CameraService.h"
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
LongRangeEnemy::~LongRangeEnemy() {
	object3d_.reset();
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void LongRangeEnemy::SetTranslate(const Vector3& pos) {baseInfo_.translate = pos;}
void LongRangeEnemy::SetlastYaw() { lastYaw_ = baseInfo_.rotate.y; }

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void LongRangeEnemy::Initialize() {

	// シードの設定
	srand(static_cast<unsigned int>(time(nullptr)));

	/// ===MoveInfoの設定=== ///
	moveInfo_.interval = 5.0f;
	moveInfo_.timer = 1.0f;
	moveInfo_.waitTime = 1.0f;
	moveInfo_.range = 10.0f;
	moveInfo_.speed = 0.05f;
	moveInfo_.direction = { 0.0f, 0.0f, 0.0f };
	moveInfo_.rangeCenter = baseInfo_.translate;
	moveInfo_.isWating = false;

	/// ===AttackInfoの設定=== ///
	attackInfo_.range = 2.0f;
	attackInfo_.distance = 10.0f;
	attackInfo_.interval = 8.0f;
	attackInfo_.timer = attackInfo_.interval;
	attackInfo_.power = 1;
	attackInfo_.direction = { 0.0f, 0.0f, 0.0f };
	attackInfo_.isAttack = false;

	/// ===Object3d=== ///
	// Object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(ObjectType::Model, "player");
	// Object3dの初期設定
	object3d_->SetTranslate(baseInfo_.translate);
	object3d_->SetRotate(baseInfo_.rotate);
	object3d_->SetScale(baseInfo_.scale);
	object3d_->SetColor(baseInfo_.color);

	/// ===SphereCollider=== ///
	// Sphereの設定
	OBBCollider::Initialize();
	name_ = ColliderName::Enemy;
	obb_.halfSize = { 2.0f, 2.0f, 2.0f };

	// BaseEnemyの初期化
	BaseEnemy::Initialize();
	
	// 攻撃用の前フレームを初期化
	lastYaw_ = baseInfo_.rotate.y;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void LongRangeEnemy::Update() {

	if (!player_) return;

	// BaseEnemyの更新
	BaseEnemy::Update();

	// 弾を更新（null ならスキップ）
	for (auto& bullet : bulletInfo_.bullets_) {
		if (bullet) bullet->Update();
	}

	// 死んだ弾をリストから削除
	bulletInfo_.bullets_.remove_if(
		[](const std::unique_ptr<LongRangeEnemeyBullet>& b) {
			return !b || !b->GetIsAlive();
		}
	);
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void LongRangeEnemy::Draw(BlendMode mode) {
	// Enemyの描画
	BaseEnemy::Draw(mode);

	// バレットの描画
	for (auto& bullet : bulletInfo_.bullets_) {
		if (bullet) bullet->Draw(mode);
	}
}

///-------------------------------------------/// 
/// 更新（ImGui）
///-------------------------------------------///
void LongRangeEnemy::UpdateImGui() {
#ifdef _DEBUG
	ImGui::Begin("LongRangeEnemy");
	BaseEnemy::UpdateImGui();
	ImGui::End();
#endif // _DEBUG
}

///-------------------------------------------/// 
/// 衝突判定
///-------------------------------------------///
void LongRangeEnemy::OnCollision(Collider* collider) {
	// BaseEnemyの当たり判定
	BaseEnemy::OnCollision(collider);
}

///-------------------------------------------/// 
/// 攻撃処理
///-------------------------------------------///
void LongRangeEnemy::Attack() {

	if (!attackInfo_.isAttack) { /// ===IsAttackがfalse=== ///
		// 攻撃時はやや速めに回転
		UpdateRotationTowards(attackInfo_.direction, 0.2f);
		// 差分（絶対値）
		float diff = std::fabs(baseInfo_.rotate.y - lastYaw_);

		// 差が小さいなら
		if (diff < 0.05f) {
			// フラグをtrue
			attackInfo_.isAttack = true;
		}
	} else {
		// 弾の生成
		auto bullet = std::make_unique<LongRangeEnemeyBullet>();
		bullet->Create(baseInfo_.translate, attackInfo_.direction);
		bulletInfo_.bullets_.push_back(std::move(bullet));
		// フラグをfalse
		attackInfo_.isAttack = false;
		baseInfo_.color = { 1.0f, 0.0f, 1.0f, 1.0f }; // 元の色に戻す（任意）
		attackInfo_.timer = attackInfo_.interval; // クールダウン再設定
	}

	// 次のフレーム用に保存
	lastYaw_ = baseInfo_.rotate.y;
}