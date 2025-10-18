#include "SphereCollider.h"
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void SphereCollider::Initialize() {
	type_ = ColliderType::Sphere;
	sphere_.center = object3d_->GetTransform().translate;

	// Colliderの初期化
	Collider::Initialize();
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void SphereCollider::Update() {
	
	/// ===Object3Dの更新=== ///
	object3d_->SetTranslate(transform_.translate);
	object3d_->SetRotate(transform_.rotate);
	object3d_->SetScale(transform_.scale);
	object3d_->SetColor(color_);

	// Sphereのセンター座標を更新
	sphere_.center = object3d_->GetTransform().translate;

	// Colliderの更新処理
	Collider::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void SphereCollider::Draw(BlendMode mode) {

	// Line
#ifdef _DEBUG
	// デバッグ時のみ描画
	line_->DrawSphere(sphere_, lineColor_);

#endif // DEBUG

	// Colliderの描画処理
	Collider::Draw(mode);
}

///-------------------------------------------/// 
/// 情報
///-------------------------------------------///
void SphereCollider::Information() {
#ifdef USE_IMGUI
	Collider::Information();
	ImGui::Text("SphereInfo");
	ImGui::DragFloat("Radius", &sphere_.radius, 0.1f, 0.0f);
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// Radius
Sphere SphereCollider::GetSphere() const {
	return sphere_;
}
