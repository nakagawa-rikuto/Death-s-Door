#include "GameCamera.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
GameCamera::~GameCamera() {
	if (type_ == CameraType::Normal) {
		normal_.reset();
	} else {
		follow_.reset();
	}
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
// WorldMatrix
const Matrix4x4& GameCamera::GetWorldMatrix() const {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		return normal_->GetWorldMatrix();
	} else {
		// Followの削除
		return follow_->GetWorldMatrix();
	}
}
// ViewMatrix
const Matrix4x4& GameCamera::GetViewMatrix() const {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		return normal_->GetViewMatrix();
	} else {
		// Followの削除
		return follow_->GetViewMatrix();
	}
}
// ProjectionMatrix
const Matrix4x4& GameCamera::GetProjectionMatrix() const {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		return normal_->GetProjectionMatrix();
	} else {
		// Followの削除
		return follow_->GetProjectionMatrix();
	}
}
// ViewProjectionMatrix
const Matrix4x4& GameCamera::GetViewProjectionMatrix() const {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		return normal_->GetViewProjectionMatrix();
	} else {
		// Followの削除
		return follow_->GetViewProjectionMatrix();
	}
}
// Translate
const Vector3& GameCamera::GetTranslate() const {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		return normal_->GetTranslate();
	} else {
		// Followの削除
		return follow_->GetTranslate();
	}
}
// Rotate
const Quaternion& GameCamera::GetRotate() const {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		return normal_->GetRotate();
	} else {
		// Followの削除
		return follow_->GetRotate();
	}
}

///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
// Translate
void GameCamera::SetTranslate(const Vector3& translate) {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		normal_->SetTranslate(translate);
	} else {
		// Followの削除
		follow_->SetTranslate(translate);
	}
}
// Rotate
void GameCamera::SetRotate(const Quaternion& rotate) {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		normal_->SetRotate(rotate);
	} else {
		// Followの削除
		follow_->SetRotate(rotate);
	}
}
// ForY
void GameCamera::SetForY(const float& forY) {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		normal_->SetForY(forY);
	} else {
		// Followの削除
		follow_->SetForY(forY);
	}
}
// AspectRatio
void GameCamera::SetAspectRatio(const float& aspect) {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		normal_->SetAspectRatio(aspect);
	} else {
		// Followの削除
		follow_->SetAspectRatio(aspect);
	}
}
// NearClip
void GameCamera::SetNearClip(const float& nearClip) {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		normal_->SetNearClip(nearClip);
	} else {
		// Followの削除
		follow_->SetNearClip(nearClip);
	}
}
// FarClip
void GameCamera::SetFarClip(const float& farClip) {
	if (type_ == CameraType::Normal) {
		// Normalの削除
		normal_->SetFarClip(farClip);
	} else {
		// Followの削除
		follow_->SetFarClip(farClip);
	}
}
// FollowCameraの設定
void GameCamera::SetFollowCamera(FollowCameraType type) {
	if (type_ == CameraType::Normal) { return; }
	follow_->SetFollowCamera(type);
}
// 追従対象の座標を設定
void GameCamera::SetTarget(Vector3* position, Quaternion* rotation) {
	if (type_ == CameraType::Normal) { return; }
	follow_->SetTarget(position, rotation);
}
// 追従のオフセット
void GameCamera::SetOffset(const Vector3& offset) {
	if (type_ == CameraType::Normal) { return; }
	follow_->SetOffset(offset);
}
void GameCamera::SetOrbitingOffset(const Vector3& offset) {
	if (type_ == CameraType::Normal) { return; }
	follow_->SetOrbitingOffset(offset);
}
// 追従速度を設定
void GameCamera::SetFollowSpeed(float speed) {
	if (type_ == CameraType::Normal) { return; }
	follow_->SetFollowSpeed(speed);
}
// 回転補間速度
void GameCamera::SetLerpSpeed(float speed) {
	if (type_ == CameraType::Normal) { return; }
	follow_->SetLerpSpeed(speed);
}
// 回転の重み
void GameCamera::SetStick(const Vector2& stickValue) {
	if (type_ == CameraType::Normal) { return; }
	follow_->SetStick(stickValue);
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void GameCamera::Init(CameraType type) {
	type_ = type;

	if (type_ == CameraType::Normal) {
		normal_ = std::make_shared<NormalCaemra>();
		normal_->Initialize();
	} else {
		follow_ = std::make_shared<FollowCamera>();
		follow_->Initialize();
	}
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void GameCamera::Update() {
	if (type_ == CameraType::Normal) {
		normal_->Update();
	} else {
		follow_->Update();
	}
}

///-------------------------------------------/// 
/// 情報
///-------------------------------------------///
void GameCamera::ImGuiUpdate() {
#ifdef USE_IMGUI
	if (type_ == CameraType::Normal) {
		normal_->ImGuiUpdate();
	} else {
		follow_->ImGuiUpdate();
	}
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// デバッグ用の更新
///-------------------------------------------///
void GameCamera::DebugUpdate() {
#ifdef _DEBUG
	if (type_ == CameraType::Normal) {
		normal_->DebugUpdate();
	} else {
		follow_->DebugUpdate();
	}
#endif // _DEBUG
}

///-------------------------------------------/// 
/// シェイク開始処理
///-------------------------------------------///
void GameCamera::StartShake(float intensity, float duration, float frequency) {
	if (type_ == CameraType::Normal) {
		normal_->StartShake(intensity, duration, frequency);
	} else {
		follow_->StartShake(intensity, duration, frequency);
	}
}

///-------------------------------------------/// 
/// シェイク処理終了
///-------------------------------------------///
void GameCamera::StopShake() {
	if (type_ == CameraType::Normal) {
		normal_->StopShake();
	} else {
		follow_->StopShake();
	}
}