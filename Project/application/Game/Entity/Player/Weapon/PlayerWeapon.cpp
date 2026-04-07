#include "PlayerWeapon.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Math
#include "Math/sMath.h"
#include "Math/EasingMath.h"
// Service
#include "Service/Collision.h"
#include "Service/DeltaTime.h"
#include "Service/Particle.h"
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
/// 初期化
///-------------------------------------------///
void PlayerWeapon::Initialize() {

	// Object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Init(std::make_unique<MiiEngine::Model>(), "PlayerWeapon");
	object3d_->SetRotate({ 0.0f, 40.0f, 0.0f, 1.0f });

	/// ===OBBCollider=== ///
	OBBCollider::Initialize();
	name_ = MiiEngine::ColliderName::PlayerWeapon;
	OBBCollider::SetHalfSize({ 2.0f, 0.5f, 5.0f });

	// DeltaTime初期化
	baseInfo_.deltaTime = Service::DeltaTime::GetDeltaTime();

	// 初期状態では非アクティブ
	SetActive(false);
	attackInfo_.isAttacking = false;

	// 更新
	Update();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void PlayerWeapon::Update() {
	// DeltaTime更新
	baseInfo_.deltaTime = Service::DeltaTime::GetDeltaTime();

	// 攻撃中でない場合は早期リターン
	if (!attackInfo_.isAttacking) {
		SetActive(false);
		OBBCollider::Update();
		return;
	}

	// タイマーを進める
	attackInfo_.timer += baseInfo_.deltaTime;
	attackInfo_.progress = attackInfo_.timer / attackInfo_.duration;

	// 攻撃終了チェック
	if (attackInfo_.progress >= 1.0f) {
		attackInfo_.isAttacking = false;
		attackInfo_.progress = 1.0f;
		SetActive(false);
		Service::Collision::RemoveCollider(this);
		OBBCollider::Update();
		// パーティクルの削除
		attackParticle_->Stop();
		attackParticle_ = nullptr;
		return;
	}

	// ベジェ曲線に沿った軌道更新
	UpdateBezierTrajectory();

	// Particleの軌道更新
	if (attackParticle_) {
		attackParticle_->SetEmitterPosition(object3d_->GetWorldTranslate());
	}
	

	/// ===OBBCollider=== ///
	OBBCollider::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void PlayerWeapon::Draw(MiiEngine::BlendMode mode) {
	// 攻撃中のみ描画
	if (attackInfo_.isAttacking) {
		OBBCollider::Draw(mode);
	}
}

///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void PlayerWeapon::Information() {
#ifdef USE_IMGUI
	ImGui::Begin("PlayerWeapon");

	ImGui::SeparatorText("攻撃情報");
	ImGui::Checkbox("攻撃フラグ", &attackInfo_.isAttacking);
	ImGui::DragFloat("タイマー", &attackInfo_.timer, 0.01f);
	ImGui::DragFloat("進行度", &attackInfo_.progress, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("持続時間", &attackInfo_.duration, 0.01f);

	ImGui::SeparatorText("ベジェ曲線");
	ImGui::Text("制御点数: %zu", attackInfo_.trajectoryPoints.size());

	for (size_t i = 0; i < attackInfo_.trajectoryPoints.size(); ++i) {
		ImGui::PushID(static_cast<int>(i));
		ImGui::Text("制御点 %zu", i);
		ImGui::Text("  位置: (%.2f, %.2f, %.2f)",
			attackInfo_.trajectoryPoints[i].position.x,
			attackInfo_.trajectoryPoints[i].position.y,
			attackInfo_.trajectoryPoints[i].position.z);
		ImGui::Text("  時間: %.2f", attackInfo_.trajectoryPoints[i].time);
		ImGui::PopID();
	}

	ImGui::End();
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// 衝突判定
///-------------------------------------------///
void PlayerWeapon::OnCollision(Collider* collider) {
	// 攻撃中でない、または既にヒット済みの場合は処理しない
	if (!baseInfo_.isActive) {
		return;
	}

	// 敵に当たった場合
	if (collider->GetColliderName() == MiiEngine::ColliderName::Enemy) {
		// ヒットフラグを立てる（1回の攻撃で複数ヒットを防ぐ）
		attackInfo_.hasHit = true;
		// ここでヒットエフェクトやサウンドの再生などを行う
	}
}

///-------------------------------------------/// 
/// 親子関係の設定
///-------------------------------------------///
void PlayerWeapon::SetUpParent(Player* parent) {
	player_ = parent;
	// 親子関係の設定
	object3d_->SetParent(player_->GetModelCommon());
}

//-------------------------------------------/// 
/// 攻撃開始処理（ベジェ曲線版）
///-------------------------------------------///
void PlayerWeapon::StartAttack(
	const std::vector<MiiEngine::BezierControlPointData>& trajectoryPoints,
	float duration) {

	// 制御点が2点未満の場合はエラー
	if (trajectoryPoints.size() < 2) {
		return;
	}

	// 攻撃情報の設定
	attackInfo_.isAttacking = true;
	attackInfo_.timer = 0.0f;
	attackInfo_.duration = duration;
	attackInfo_.progress = 0.0f;

	// ベジェ曲線の制御点を保存
	attackInfo_.trajectoryPoints = trajectoryPoints;

	// コライダーに追加
	Service::Collision::AddCollider(this);

	// 当たり判定フラグをリセット
	attackInfo_.hasHit = false;

	// コライダーを有効化
	SetActive(true);

	// Particleの開始
	if (attackParticle_) {
		attackParticle_->Stop();
		attackParticle_ = nullptr;
	}
	attackParticle_ = Service::Particle::Emit("PlayerAttack", trajectoryPoints.front().position);
	attackParticle_->SetEmitterPosition(object3d_->GetWorldTranslate());

	// 初期位置と回転を設定
	transform_.translate = trajectoryPoints.front().position;
	transform_.rotate = trajectoryPoints.front().rotation;
}

///-------------------------------------------/// 
/// ベジェ曲線上の位置を計算
///-------------------------------------------///
Vector3 PlayerWeapon::CalculateBezierPoint(const std::vector<MiiEngine::BezierControlPointData>& controlPoints, float t) {
	size_t n = controlPoints.size();

	// 制御点が2点の場合は線形補間
	if (n == 2) {
		return Math::Lerp(controlPoints[0].position, controlPoints[1].position, t);
	}

	// 制御点が3点の場合は2次ベジェ曲線
	if (n == 3) {
		float u = 1.0f - t;
		float uu = u * u;
		float tt = t * t;

		Vector3 point = {};
		point.x = uu * controlPoints[0].position.x +
			2.0f * u * t * controlPoints[1].position.x +
			tt * controlPoints[2].position.x;
		point.y = uu * controlPoints[0].position.y +
			2.0f * u * t * controlPoints[1].position.y +
			tt * controlPoints[2].position.y;
		point.z = uu * controlPoints[0].position.z +
			2.0f * u * t * controlPoints[1].position.z +
			tt * controlPoints[2].position.z;

		return point;
	}

	// 制御点が4点の場合は3次ベジェ曲線
	if (n == 4) {
		float u = 1.0f - t;
		float uu = u * u;
		float uuu = uu * u;
		float tt = t * t;
		float ttt = tt * t;

		Vector3 point = {};
		point.x = uuu * controlPoints[0].position.x +
			3.0f * uu * t * controlPoints[1].position.x +
			3.0f * u * tt * controlPoints[2].position.x +
			ttt * controlPoints[3].position.x;
		point.y = uuu * controlPoints[0].position.y +
			3.0f * uu * t * controlPoints[1].position.y +
			3.0f * u * tt * controlPoints[2].position.y +
			ttt * controlPoints[3].position.y;
		point.z = uuu * controlPoints[0].position.z +
			3.0f * uu * t * controlPoints[1].position.z +
			3.0f * u * tt * controlPoints[2].position.z +
			ttt * controlPoints[3].position.z;

		return point;
	}

	// 5点以上の場合はDe Casteljauのアルゴリズム
	std::vector<Vector3> temp;
	for (const auto& cp : controlPoints) {
		temp.push_back(cp.position);
	}

	while (temp.size() > 1) {
		std::vector<Vector3> newTemp;
		for (size_t i = 0; i < temp.size() - 1; ++i) {
			newTemp.push_back(Math::Lerp(temp[i], temp[i + 1], t));
		}
		temp = newTemp;
	}

	return temp[0];
}

///-------------------------------------------/// 
/// ベジェ曲線上の回転を計算
///-------------------------------------------///
Quaternion PlayerWeapon::CalculateBezierRotation(const std::vector<MiiEngine::BezierControlPointData>& controlPoints, float t) {
	size_t n = controlPoints.size();
	if (n == 0) {
		// デフォルト quaternion（必要に応じて変更）
		return Quaternion{ 0, 0, 0, 1 };
	}
	if (n == 1) {
		return controlPoints[0].rotation;
	}

	// 2点の場合→SLerp
	if (n == 2) {
		return Math::SLerp(controlPoints[0].rotation, controlPoints[1].rotation, t);
	}

	// 3点以上→De Casteljau法   (SLerpで再帰的に作る)
	std::vector<Quaternion> temp;
	for (const auto& cp : controlPoints) {
		temp.push_back(cp.rotation);
	}

	// 再帰的補間
	while (temp.size() > 1) {
		std::vector<Quaternion> nextLayer;
		for (size_t i = 0; i < temp.size() - 1; ++i) {
			nextLayer.push_back(Math::SLerp(temp[i], temp[i + 1], t));
		}
		temp = nextLayer;
	}
	return temp[0];
}

///-------------------------------------------/// 
/// ベジェ曲線に沿った軌道更新
///-------------------------------------------///
void PlayerWeapon::UpdateBezierTrajectory() {
	// イージング関数を適用（滑らかな加速・減速）
	float t = attackInfo_.progress;
	float easedT = Easing::EaseInOutQuad(t);

	// ベジェ曲線上の位置を計算
	transform_.translate = CalculateBezierPoint(attackInfo_.trajectoryPoints, easedT);
	// ベジェ曲線上の回転を計算
	transform_.rotate = CalculateBezierRotation(attackInfo_.trajectoryPoints, easedT);
}