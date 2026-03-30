#include "GameCharacter.h"
// DeltaTime
#include "Service/DeltaTime.h"
// Collider
#include "Engine/Collider/AABBCollider.h"
// C++
#include <algorithm>
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

/// ===テンプレート候補=== ///
template class GameCharacter<MiiEngine::OBBCollider>;
template class GameCharacter<MiiEngine::SphereCollider>;

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
GameCharacter<TCollider>::GameCharacter() {}

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
GameCharacter<TCollider>::~GameCharacter() {
	// 手動の解放
	this->object3d_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
void GameCharacter<TCollider>::Initialize() {
	/// ===ColliderCollision=== ///
	collision_ = std::make_unique<MiiEngine::ColliderCollision>();
	gCollision_ = std::make_unique<GameCharacterCollision>();

	/// ===BaseInfoの初期化設定=== ///
	baseInfo_.velocity = { 0.0f, 0.0f, 0.0f };
	baseInfo_.deltaTime = Service::DeltaTime::GetDeltaTime();
	baseInfo_.gravity = -9.8f;
	baseInfo_.isDead = false;

	/// ===キャラクターの半サイズY軸=== ///
	characterHalfSizeY_ = 0.0f; 

	/// ===GroundInfo=== ///
	groundInfo_.currentGroundYPos = 0.0f;
	groundInfo_.isGrounded = false;

	/// ===AreaInfo=== ///
	areaInfo_.center = { 0.0f, 0.0f, 0.0f };
	areaInfo_.halfSize = { 200.0f, 100.0f, 200.0f };

	/// ===TCollider=== ///
	TCollider::Initialize();
}

///-------------------------------------------/// 
/// 更新処理の前処理
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
void GameCharacter<TCollider>::PreUpdate() {
	/// ===死亡処理=== ///
	if (baseInfo_.HP <= 0) {
		baseInfo_.isDead = true;
	}

	/// ===地面との当たり判定フラグのリセット=== ///
	float limitY = groundInfo_.currentGroundYPos + characterHalfSizeY_;
	if (this->transform_.translate.y > limitY) {
		groundInfo_.isGrounded = false;
	}

	/// ===デルタタイムの取得=== ///
	baseInfo_.deltaTime = Service::DeltaTime::GetDeltaTime();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
void GameCharacter<TCollider>::Update() {

	/// ===地面との衝突処理=== ///
	GroundCollision();

	/// ===エリアの衝突処理=== ///
	AreaCollision();

	/// ===位置の更新=== ///
	this->transform_.translate += baseInfo_.velocity;

	/// ===TColliderの更新処理=== ///
	TCollider::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
void GameCharacter<TCollider>::Draw(MiiEngine::BlendMode mode) {
	/// ===TCollider=== ///
	TCollider::Draw(mode);
}

///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
void GameCharacter<TCollider>::Information() {
#ifdef USE_IMGUI
	TCollider::Information();
	ImGui::Text("ゲームキャラクター情報");
	ImGui::DragFloat("デルタタイム", &baseInfo_.deltaTime, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("速度", &baseInfo_.velocity.x, 0.1f);
	ImGui::DragFloat("重力", &baseInfo_.gravity, 0.1f, -20.0f, 0.0f);
	ImGui::Checkbox("死亡フラグ", &baseInfo_.isDead);
	ImGui::Checkbox("接地フラグ", &groundInfo_.isGrounded);
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 衝突
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
void GameCharacter<TCollider>::OnCollision(MiiEngine::Collider* collider) {
	// === 早期リターン === //
	if (!collider) return;

	/// ===Colliderとの衝突処理=== ///
	if (collider->GetColliderName() == MiiEngine::ColliderName::Object) {
		// Objectとの衝突処理
		//NOTE:thisを100%押し戻し
		collision_->ProcessCollision(this, collider, 0.0f);

	} else if (auto otherCharacter = dynamic_cast<GameCharacter<TCollider>*>(collider)) { //　これだと当たり判定が通った時に全て通ってしまう。

		// GameCharacterの場合の衝突処理
		//NOTE: OBBだけどSphereの押し戻しと同じ処理
		gCollision_->ProcessCollision(this, otherCharacter, 0.0f);
	}
}

///-------------------------------------------/// 
/// 地面との衝突処理
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
void GameCharacter<TCollider>::GroundCollision() {

	/// ===地面の範囲という概念を無くし、常に特定の値より下に行かないようにする=== ///

	// 地面より上にいる場合は重力を適用
	if (!groundInfo_.isGrounded) {
		baseInfo_.velocity.y += baseInfo_.gravity * baseInfo_.deltaTime;

		// 落下速度の最大値を制限
		const float kMaxFallSpeed = -10.0f;
		if (baseInfo_.velocity.y < kMaxFallSpeed) {
			baseInfo_.velocity.y = kMaxFallSpeed;
		}
	}

	// 次のフレームの予測Y座標
	float nextY = this->transform_.translate.y + baseInfo_.velocity.y;

	// limitYより下に行かないように制限
	float limitY = groundInfo_.currentGroundYPos + characterHalfSizeY_;
	if (nextY <= limitY) {
		// 地面の位置にピッタリ合わせるように速度を調整
		baseInfo_.velocity.y = limitY - this->transform_.translate.y;
		groundInfo_.isGrounded = true;
	}
}

///-------------------------------------------/// 
/// エリアの衝突処理
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
void GameCharacter<TCollider>::AreaCollision() {
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