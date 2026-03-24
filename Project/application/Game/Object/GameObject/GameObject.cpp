#include "GameObject.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

/// ===テンプレート候補=== ///
template class GameObject<MiiEngine::AABBCollider>;
template class GameObject<MiiEngine::OBBCollider>;
template class GameObject<MiiEngine::SphereCollider>;

///-------------------------------------------/// 
/// デストラクタ						
///-------------------------------------------///
template<typename TCollider> requires MiiEngine::IsCollider<TCollider>
GameObject<TCollider>::~GameObject() {				
	this->object3d_.reset();
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
template<typename TCollider> requires MiiEngine::IsCollider<TCollider>
void GameObject<TCollider>::Initialize() {
	/// ===TCollider=== ///
	TCollider::Initialize();
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
template<typename TCollider> requires MiiEngine::IsCollider<TCollider>
void GameObject<TCollider>::Update() {
	/// ===TCollider=== ///
	TCollider::Update();
}

///-------------------------------------------/// 
/// 描画処理
///-------------------------------------------///
template<typename TCollider> requires MiiEngine::IsCollider<TCollider>
void GameObject<TCollider>::Draw(MiiEngine::BlendMode mode) {
	/// ===TCollider=== ///
	TCollider::Draw(mode);
}

///-------------------------------------------/// 
/// ImGui情報の表示
///-------------------------------------------///
template<typename TCollider> requires MiiEngine::IsCollider<TCollider>
void GameObject<TCollider>::Information() {}

///-------------------------------------------/// 
/// 衝突判定
///-------------------------------------------///
template<typename TCollider> requires MiiEngine::IsCollider<TCollider>
void GameObject<TCollider>::OnCollision(MiiEngine::Collider* collider) {
	collider;
}

