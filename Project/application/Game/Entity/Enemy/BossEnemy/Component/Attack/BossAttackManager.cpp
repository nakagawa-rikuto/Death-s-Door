#include "BossAttackManager.h"
// C++
#include <cassert>
#include <algorithm>
// Math
#include <Math/sMath.h>
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

#ifdef USE_IMGUI
///-------------------------------------------///
/// 設定の適用
///-------------------------------------------///
void BossAttackManager::ApplyConfig(const Config& newConfig) {
	config_ = newConfig;
	// 各コンポーネントにも設定を反映
	rotate_->ApplyConfig(config_.rotateConfig);
	downswing_->ApplyConfig(config_.downswingConfig);
	jumpSmash_->ApplyConfig(config_.jumpSmashConfig);
}
#endif // USE_IMGUI

///-------------------------------------------///
/// 初期化処理
///-------------------------------------------///
void BossAttackManager::Initialize(const Config& config) {
	config_ = config;
	cooldowns_ = Cooldowns{};

	// 各コンポーネントの生成と初期化
	rotate_ = std::make_unique<BossAttackRotateComponent>();
	rotate_->Initialize(config_.rotateConfig);

	downswing_ = std::make_unique<BossAttackDownwardSwingComponent>();
	downswing_->Initialize(config_.downswingConfig);

	jumpSmash_ = std::make_unique<BossAttackJumpSmashComponent>();
	jumpSmash_->Initialize(config_.jumpSmashConfig);
}

///-------------------------------------------///
/// タイマー更新
///-------------------------------------------///
void BossAttackManager::UpdateTimers(float deltaTime) {
	cooldowns_.rotate = std::max(0.0f, cooldowns_.rotate - deltaTime);
	cooldowns_.downswing = std::max(0.0f, cooldowns_.downswing - deltaTime);
	cooldowns_.jumpSmash = std::max(0.0f, cooldowns_.jumpSmash - deltaTime);
}

///-------------------------------------------///
/// ImGui情報の表示
///-------------------------------------------///
void BossAttackManager::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("攻撃マネージャー")) {
		// クールダウン表示
		if (ImGui::TreeNode("クールダウン")) {
			ImGui::Text("Rotate:      %.2f / %.2f", cooldowns_.rotate, config_.rotateCooldown);
			ImGui::Text("DownSwing:   %.2f / %.2f", cooldowns_.downswing, config_.downswingCooldown);
			ImGui::Text("JumpSmash:   %.2f / %.2f", cooldowns_.jumpSmash, config_.jumpSmashCooldown);
			ImGui::TreePop();
		}

		// 射程設定
		if (ImGui::TreeNode("射程設定")) {
			ImGui::DragFloat("Thrust 射程", &config_.rotateRange, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("DownSwing 射程", &config_.downswingRange, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("JumpSmash 最小射程", &config_.jumpSmashMinRange, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("JumpSmash 最大射程", &config_.jumpSmashMaxRange, 0.1f, 0.0f, 100.0f);
			ImGui::TreePop();
		}

		// クールダウン設定
		if (ImGui::TreeNode("クールダウン設定")) {
			ImGui::DragFloat("Thrust CT", &config_.rotateCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("DownSwing CT", &config_.downswingCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("JumpSmash CT", &config_.jumpSmashCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// 各攻撃コンポーネントの詳細
		if (rotate_) rotate_->Information();
		if (downswing_) downswing_->Information();
		if (jumpSmash_) jumpSmash_->Information();

		ImGui::TreePop();
	}
#endif // USE_IMGUI
}
