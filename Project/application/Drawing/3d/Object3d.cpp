#include "Object3d.h"
// C++
#include <cassert>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
Object3d::~Object3d() {
	model_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Object3d::Init(std::unique_ptr<MiiEngine::ModelCommon> model, const std::string& modelName, MiiEngine::LightType light) {
	// 生成。
	model_ = std::move(model);
	// 初期化
	model_->Initialize(modelName, light);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Object3d::Update() {
	model_->Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void Object3d::Draw(MiiEngine::BlendMode mode) {
	model_->Draw(mode);
}

///-------------------------------------------/// 
/// 親子関係
///-------------------------------------------///
// SetParent
void Object3d::SetParent(MiiEngine::ModelCommon* parent) {
	model_->SetParent(parent);
}
// ClearParent
void Object3d::ClearParent() {
	model_->ClearParent();
}
// Offset
void Object3d::SetParentOffset(const Vector3& offset) {
	model_->SetParentOffset(offset);
} 
const Vector3& Object3d::GetParentOffset() const {
	return model_->GetParentOffset();
}


///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// Translate
const Vector3& Object3d::GetWorldTranslate() const {
	return model_->GetWorldTranslate();
}
// Rotate
const Quaternion& Object3d::GetWorldRotate() const {
	return model_->GetWorldRotate();
}
// Scale
const Vector3& Object3d::GetWorldScale() const {
	return model_->GetWorldScale();
}
// Transform
const MiiEngine::QuaternionTransform& Object3d::GetWorldTransform() const {
	return model_->GetWorldTransform();
}
// Color
const Vector4& Object3d::GetColor() const {
	return model_->GetColor();
}
// ModelCommon
MiiEngine::ModelCommon* Object3d::GetModelCommon() {
	return model_.get();
}


///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Translate
void Object3d::SetTranslate(const Vector3& translate) {
	model_->SetTranslate(translate);
}
// Rotate
void Object3d::SetRotate(const Quaternion& rotate) {
	model_->SetRotate(rotate);
}
// Scale
void Object3d::SetScale(const Vector3& scale) {
	model_->SetScale(scale);
}
// Color
void Object3d::SetColor(const Vector4& color) {
	model_->SetColor(color);
}
// LightInfo
void Object3d::SetLightData(MiiEngine::LightInfo light) {
	model_->SetLightData(light);
}
// Light
void Object3d::SetLight(MiiEngine::LightType type) {
	model_->SetLightType(type);
}
// 環境マップ
void Object3d::SetEnvironmentMapData(bool flag, float string) {
	model_->SetEnvironmentMapData(flag, string);
}