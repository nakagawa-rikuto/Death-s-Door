#include "Object3d.h"

#include <cassert>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
Object3d::~Object3d() {
	if (type_ == ObjectType::Model) {
		model_.reset();
	} else {
		animationModel_.reset();
	}
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void Object3d::Init(ObjectType type, const std::string& modelName, LightType light) {
	// タイプの確認
	type_ = type;
	// それぞれ対応した方を生成
	if (type_ == ObjectType::Model) {
		model_ = std::make_unique<Model>();
		model_->Initialize(modelName, light);
	} else {
		animationModel_ = std::make_unique<AnimationModel>();
		animationModel_->Initialize(modelName, light);
	}
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void Object3d::Update() {
	if (type_ == ObjectType::Model) {
		model_->Update();
	} else {
		animationModel_->Update();
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void Object3d::Draw(BlendMode mode) {
	if (type_ == ObjectType::Model) {
		model_->Draw(mode);
	} else {
		animationModel_->Draw(mode);
	}
}

///-------------------------------------------/// 
/// 親子関係
///-------------------------------------------///
void Object3d::SetParent(ModelCommon* parent) {
	if (type_ == ObjectType::Model) {
		model_->SetParent(parent);
	} else {
		animationModel_->SetParent(parent);
	}
}
void Object3d::ClearParent() {
	if (type_ == ObjectType::Model) {
		model_->ClearParent();
	} else {
		animationModel_->ClearParent();
	}
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// Translate
const Vector3& Object3d::GetTranslate() const {
	if (type_ == ObjectType::Model) {
		return model_->GetTranslate();
	} else {
		return animationModel_->GetTranslate();
	}
}
// Rotate
const Quaternion& Object3d::GetRotate() const {
	if (type_ == ObjectType::Model) {
		return model_->GetRotate();
	} else {
		return animationModel_->GetRotate();
	}
}
// Scale
const Vector3& Object3d::GetScale() const {
	if (type_ == ObjectType::Model) {
		return model_->GetScale();
	} else {
		return animationModel_->GetScale();
	}
}
// Color
const Vector4& Object3d::GetColor() const {
	if (type_ == ObjectType::Model) {
		return model_->GetColor();
	} else {
		return animationModel_->GetColor();
	}
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Translate
void Object3d::SetTranslate(const Vector3& translate) {
	if (type_ == ObjectType::Model) {
		model_->SetTranslate(translate);
	} else {
		animationModel_->SetTranslate(translate);
	}
}
// Rotate
void Object3d::SetRotate(const Quaternion& rotate) {
	if (type_ == ObjectType::Model) {
		model_->SetRotate(rotate);
	} else {
		animationModel_->SetRotate(rotate);
	}
}
// Scale
void Object3d::SetScale(const Vector3& scale) {
	if (type_ == ObjectType::Model) {
		model_->SetScale(scale);
	} else {
		animationModel_->SetScale(scale);
	}
}
// Color
void Object3d::SetColor(const Vector4& color) {
	if (type_ == ObjectType::Model) {
		model_->SetColor(color);
	} else {
		animationModel_->SetColor(color);
	}
}
// LightInfo
void Object3d::SetLightData(LightInfo light) { 
	if (type_ == ObjectType::Model) {
		model_->SetLightData(light);
	} else {
		animationModel_->SetLightData(light);
	}
}
// Light
void Object3d::SetLight(LightType type) {
	if (type_ == ObjectType::Model) {
		model_->SetLight(type);
	} else {
		animationModel_->SetLight(type);
	}
}
// 環境マップ
void Object3d::SetEnviromentMapData(bool flag, float string) {
	if (type_ == ObjectType::Model) {
		model_->SetEnviromentMapData(flag, string);
	} else {
		animationModel_->SetEnviromentMapData(flag, string);
	}
}
// Animation
void Object3d::SetAnimation(const std::string& animationName, bool isLoop) {
	if (type_ == ObjectType::AnimationModel) {
		animationModel_->SetAnimation(animationName, isLoop);
	} else {
		// タイプがアニメーションモデル出なければ強制的に止める
		assert(0);
	}
}
