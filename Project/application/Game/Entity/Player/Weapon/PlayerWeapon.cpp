#include "PlayerWeapon.h"
// Player
#include "application/Game/Entity/Player/Player.h"

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

	// 更新
	Update();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void PlayerWeapon::Update() {

	/// ===OBBCollider=== ///
	OBBCollider::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void PlayerWeapon::Draw(BlendMode mode) {

	/// ===OBBCollider=== ///
	OBBCollider::Draw(mode);
}

///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void PlayerWeapon::Information() {
#ifdef USE_IMGUI
	ImGui::Begin("PlayerWeapon");
	OBBCollider::Information();
	ImGui::End();
#endif // USE_IMGUI

}

///-------------------------------------------/// 
/// 衝突判定
///-------------------------------------------///
void PlayerWeapon::OnCollision(Collider * collider) {

}
