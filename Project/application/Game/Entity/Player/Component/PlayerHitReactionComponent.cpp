#include "PlayerHitReactionComponent.h"
// Math
#include "Math/sMath.h"

#ifdef USE_IMGUI
#include "imgui.h"
#endif

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void PlayerHitReactionComponent::Initialize(const HitConfig& config) {
	config_ = config;
	currentKnockBackDirection_ = { 0.0f, 0.0f, 0.0f };
}

///-------------------------------------------/// 
/// ヒットリアクションの開始
///-------------------------------------------///
PlayerHitReactionComponent::UpdateResult PlayerHitReactionComponent::Start(const Vector3& knockbackDirection) {
	// ノックバック方向の正規化（y成分は0にして水平ノックバックのみ）
	currentKnockBackDirection_ = knockbackDirection;
	currentKnockBackDirection_.y = 0.0f; // 水平方向のみノックバック
	if (currentKnockBackDirection_.x != 0.0f || currentKnockBackDirection_.z != 0.0f) {
		currentKnockBackDirection_ = Normalize(currentKnockBackDirection_);
	}

	// 更新結果の作成
	UpdateResult result{};
	result.velocity = currentKnockBackDirection_ * config_.knockbackSpeed;
	return result;
}

///-------------------------------------------/// 
/// ImGui
///-------------------------------------------///
void PlayerHitReactionComponent::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("HitReaction")) {
		ImGui::DragFloat("Knockback Speed", &config_.knockbackSpeed, 0.1f);
		ImGui::TreePop();
	}
#endif
}

