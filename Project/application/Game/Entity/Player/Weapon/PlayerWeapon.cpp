#include "PlayerWeapon.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include "Engine/System/Service/ColliderService.h"
// Math
#include "Math/sMath.h"
#include "Math/EasingMath.h"

// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
PlayerWeapon::~PlayerWeapon() {
	object3d_.reset();
}

///-------------------------------------------/// 
/// Player用の初期化
///-------------------------------------------///
void PlayerWeapon::InitPlayer(Player* player) {
	// Playerの設定
	player_ = player;

	Initialize();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void PlayerWeapon::Initialize() {

	// Object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->SetParent(player_->GetModelCommon());
	object3d_->Init(ObjectType::Model, "MonsterBall");

	/// ===OBBCollider=== ///
	OBBCollider::Initialize();
	name_ = ColliderName::PlayerWeapon;
	obb_.halfSize = { 0.5f, 1.5f, 0.5f };

	// 初期オフセット設定
	baseInfo_.offset_ = { 1.5f, 1.0f, 0.0f };

	// 更新
	Update();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void PlayerWeapon::Update() {
	// 攻撃中の場合のみ当たり判定を有効化
	if (attackInfo_.isAttack) {
		// 攻撃タイマーを進める
		attackInfo_.timer += player_->GetDeltaTime();

		// 攻撃時間が終了したら
		if (attackInfo_.timer >= attackInfo_.duration) {
			attackInfo_.isAttack = false;
			attackInfo_.timer = 0.0f;
		}
	}

	// プレイヤーの位置と回転を基準にオフセットを適用
	Vector3 playerPos = player_->GetTransform().translate;
	Quaternion playerRot = player_->GetTransform().rotate;

	// オフセットをプレイヤーの回転に合わせて回転
	Vector3 rotatedOffset = Math::RotateVector(baseInfo_.offset_, playerRot);
	transform_.translate = playerPos + rotatedOffset;
	transform_.rotate = playerRot;

	/// ===OBBCollider=== ///
	OBBCollider::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void PlayerWeapon::Draw(BlendMode mode) {
	// 攻撃中のみ描画
	if (attackInfo_.isAttack) {
		/// ===OBBCollider=== ///
		OBBCollider::Draw(mode);
	}
}

///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void PlayerWeapon::Information() {
#ifdef USE_IMGUI
	ImGui::Begin("PlayerWeapon");
	OBBCollider::Information();
	ImGui::DragFloat3("Offset", &baseInfo_.offset_.x, 0.1f);
	ImGui::Checkbox("IsAttack", &attackInfo_.isAttack);
	ImGui::DragFloat("Timer", &attackInfo_.timer, 0.01f);
	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 衝突判定
///-------------------------------------------///
void PlayerWeapon::OnCollision(Collider* collider) {
	// 攻撃中かつ敵との衝突
	if (attackInfo_.isAttack && collider->GetColliderName() == ColliderName::Enemy) {
		// ヒット処理（必要に応じて実装）
	}
}

///-------------------------------------------/// 
/// 攻撃処理
///-------------------------------------------///
void PlayerWeapon::Attack(const Vector3& startPoint, const Vector3& endPoint, float time) {
	attackInfo_.isAttack = true;
	attackInfo_.timer = 0.0f;
	attackInfo_.duration = time;
	attackInfo_.startOffset = startPoint;
	attackInfo_.endOffset = endPoint;

	// 開始位置を設定
	baseInfo_.offset_ = startPoint;
}

///-------------------------------------------/// 
/// 攻撃の回転を計算
///-------------------------------------------///
void PlayerWeapon::CalculateAttackRotation(const Vector3& startPoint, const Vector3& endPoint, float time) {
	// 攻撃の進行度を計算 (0.0 ~ 1.0)
	float t = attackInfo_.timer / time;

	// イージング関数を適用（accelerate-decelerate）
	t = t * t * (3.0f - 2.0f * t);

	// 開始点から終了点へ補間
	baseInfo_.offset_ = Math::Lerp(startPoint, endPoint, t);
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
bool PlayerWeapon::GetIsAttack() const {
	return attackInfo_.isAttack;
}