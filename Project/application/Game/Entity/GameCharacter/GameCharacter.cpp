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

	/// ===GroundInfo=== ///
	groundInfo_.isGrounded = false;

	/// ===TCollider=== ///
	TCollider::Initialize();
}

///-------------------------------------------/// 
/// 更新処理の前処理
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
void GameCharacter<TCollider>::PreUpdate() {
	/// ===死亡処理=== ///
	if (baseInfo_.HP <= 0 || this->transform_.translate.y < -10.0f) {
		baseInfo_.isDead = true;
	}

	/// ===フラグのリセット=== ///
	if (this->transform_.translate.y > groundInfo_.currentGroundYPos) {
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
	if (collider->GetColliderName() == MiiEngine::ColliderName::Ground) {

		// 地面に接地
		groundInfo_.isGrounded = true;

		// 地面に衝突した際の処理
		GroundOnCollision(collider);

	} else if (collider->GetColliderName() == MiiEngine::ColliderName::Object) {
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

	/// ===地面から離れている場合の処理=== ///
	if (!groundInfo_.isGrounded) {
		// 重力の適用
		baseInfo_.velocity.y += baseInfo_.gravity * baseInfo_.deltaTime;

		// Y方向の速度の最大値を制限
		const float kMaxFullSpeed = -10.0f;
		// 下方向への速度を制限
		baseInfo_.velocity.y = std::clamp(baseInfo_.velocity.y, kMaxFullSpeed, 0.0f);

		// 早期リターン
		return;
	}

	/// ===地面より下に行かないようにする=== ///
	if (this->transform_.translate.y < groundInfo_.currentGroundYPos) {

		// はみ出し分を押し戻す
		this->transform_.translate.y = groundInfo_.currentGroundYPos;

		// 下降中なら速度を0にする
		if (baseInfo_.velocity.y < 0.0f) {
			baseInfo_.velocity.y = 0.0f;
		}
	}

	/// ===地面の範囲を出たかチェック=== ///
	bool isOutOfRange = false;

	if (groundInfo_.currentGroundType == MiiEngine::ColliderType::AABB) {
		// min～maxの範囲
		float minX = groundInfo_.currentGroundFirst.x;
		float maxX = groundInfo_.currentGroundSecond.x;
		float minZ = groundInfo_.currentGroundFirst.z;
		float maxZ = groundInfo_.currentGroundSecond.z;

		isOutOfRange = (this->transform_.translate.x > maxX || this->transform_.translate.x < minX ||
			this->transform_.translate.z > maxZ || this->transform_.translate.z < minZ);

	} else if (groundInfo_.currentGroundType == MiiEngine::ColliderType::OBB) {
		// center +- halfSize
		float centerX = groundInfo_.currentGroundFirst.x;
		float halfSizeX = groundInfo_.currentGroundSecond.x;
		float centerZ = groundInfo_.currentGroundFirst.z;
		float halfSizeZ = groundInfo_.currentGroundSecond.z;

		isOutOfRange = (this->transform_.translate.x > centerX + halfSizeX || this->transform_.translate.x < centerX - halfSizeX ||
			this->transform_.translate.z > centerZ + halfSizeZ || this->transform_.translate.z < centerZ - halfSizeZ);
	}

	// 地面の範囲から出たらフラグをリセット
	if (isOutOfRange) {
		groundInfo_.isGrounded = false;
	}
}

///-------------------------------------------/// 
/// 地面に衝突した際の処理
///-------------------------------------------///
template<typename TCollider> requires IsCollider<TCollider>
void GameCharacter<TCollider>::GroundOnCollision(MiiEngine::Collider* collider) {

	// タイプの取得
	groundInfo_.currentGroundType = collider->GetColliderType();

	/// ===タイプ別の地面情報の取得=== ///
	if (groundInfo_.currentGroundType == MiiEngine::ColliderType::AABB) { // AABBの場合
		MiiEngine::AABBCollider* aabbCollider = dynamic_cast<MiiEngine::AABBCollider*>(collider);
		if (aabbCollider) {
			MiiEngine::AABB groundAABB = aabbCollider->GetAABB();
			// 地面の情報を保存(Min, Max)
			groundInfo_.currentGroundFirst = groundAABB.min;
			groundInfo_.currentGroundSecond = groundAABB.max;
		}
	} else if (groundInfo_.currentGroundType == MiiEngine::ColliderType::OBB) { // OBBの場合
		MiiEngine::OBBCollider* obbCollider = dynamic_cast<MiiEngine::OBBCollider*>(collider);
		if (obbCollider) {
			MiiEngine::OBB groundOBB = obbCollider->GetOBB();
			// 地面の情報を保存(Center, HalfSize)
			groundInfo_.currentGroundFirst = groundOBB.center;
			groundInfo_.currentGroundSecond = groundOBB.halfSize;
		}
	}

	/// ===GameCharacterの情報を取得=== ///
	float characterHalfSizeY = 0.0f;
	if (this->GetColliderType() == MiiEngine::ColliderType::OBB) {
		MiiEngine::OBB obb = dynamic_cast<MiiEngine::OBBCollider*>(this)->GetOBB();
		characterHalfSizeY = obb.halfSize.y;
		
	} else if (this->GetColliderType() == MiiEngine::ColliderType::Sphere) {
		MiiEngine::Sphere sphere = dynamic_cast<MiiEngine::SphereCollider*>(this)->GetSphere();
		characterHalfSizeY = sphere.radius;
	}

	/// ===地面のY座標を計算（地面タイプに応じて）=== ///
	float groundTopY = 0.0f;
	if (groundInfo_.currentGroundType == MiiEngine::ColliderType::AABB) {
		// AABBの場合: maxがそのまま上端
		groundTopY = groundInfo_.currentGroundSecond.y;

	} else if (groundInfo_.currentGroundType == MiiEngine::ColliderType::OBB) {
		// OBBの場合: center + halfSize
		groundTopY = groundInfo_.currentGroundFirst.y + groundInfo_.currentGroundSecond.y;
	}

	/// ===押し戻し後のY座標を計算=== ///
	groundInfo_.currentGroundYPos = groundTopY + characterHalfSizeY;
}
