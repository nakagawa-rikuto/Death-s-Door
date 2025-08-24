#include "Collider.h"

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
// Light
void Collider::SetLight(LightType type) { object3d_->SetLight(type); }
void Collider::SetLightData(LightInfo light) { object3d_->SetLightData(light); }
 
// IsCollision
void Collider::SetIsCollisison(bool flag) { isCollision_ = flag; }

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Collider::Initialize() {
#ifdef _DEBUG
	line_ = std::make_unique<Line>();
#endif // _DEBUG

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
