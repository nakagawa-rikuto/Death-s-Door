#include "Collider.h"
// ImGui
#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// Type
ColliderType Collider::GetColliderType() { return type_; }
// Name
ColliderName Collider::GetColliderName() { return name_; }


///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Object3D
void Collider::SetTranslate(const Vector3& translate) { transform_.translate = translate; }
void Collider::SetRotate(const Quaternion& rotate) { transform_.rotate = rotate; }
void Collider::SetScale(const Vector3& scale) { transform_.scale = scale; }
void Collider::SetColor(const Vector4& color) { color_ = color; }
// Light
void Collider::SetLight(LightType type) { object3d_->SetLight(type); }
void Collider::SetLightData(LightInfo light) { object3d_->SetLightData(light); }
// IsCollision
void Collider::SetIsCollisison(bool flag) { isCollision_ = flag; }

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// Transform
QuaternionTransform Collider::GetTransform() const { return transform_; }
// Color
Vector4 Collider::GetColor() const { return color_; }
// IsCollision
bool Collider::GetIsCollision() const { return isCollision_; }


///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Collider::Initialize() {
#ifdef _DEBUG
	line_ = std::make_unique<Line>();
#endif // _DEBUG

	/// ===Transformの初期設定=== ///
	transform_.translate = { 0.0f, 0.0f, 0.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f, 1.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };
	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
}


///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void Collider::Update() {

#ifdef _DEBUG
	// 線の色を更新
	if (isCollision_) {
		lineColor_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	} else {
		lineColor_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}
#endif // _DEBUG	

	// Object3Dの更新
	object3d_->Update();
}


///-------------------------------------------/// 
/// 描画処理
///-------------------------------------------///
void Collider::Draw(BlendMode mode) {
	// Object3Dの描画
	object3d_->Draw(mode);
}

///-------------------------------------------/// 
/// 情報
///-------------------------------------------///
void Collider::Information() {
#ifdef USE_IMGUI
	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat4("Rotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f);
	ImGui::ColorEdit4("Color", &color_.x);
#endif // USE_IMGUI

}