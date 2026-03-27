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
	assert(newConfig.thrustRange > 0.0f && "thrustRange must be > 0");
	assert(newConfig.downswingRange > 0.0f && "downswingRange must be > 0");
	assert(newConfig.jumpSmashRange > 0.0f && "jumpSmashRange must be > 0");
	config_ = newConfig;
	// 各コンポーネントにも設定を反映
	thrust_->ApplyConfig(config_.thrustConfig);
	downswing_->ApplyConfig(config_.downswingConfig);
	jumpSmash_->ApplyConfig(config_.jumpSmashConfig);
}
#endif // USE_IMGUI

///-------------------------------------------///
/// 初期化処理
///-------------------------------------------///
void BossAttackManager::Initialize(const Config& config) {
	config_ = config;
	currentAttack_ = AttackType::None;
	cooldowns_ = Cooldowns{};

	// 各コンポーネントの生成と初期化
	thrust_ = std::make_unique<BossAttackThrustComponent>();
	thrust_->Initialize(config_.thrustConfig);

	downswing_ = std::make_unique<BossAttackDownwardSwingComponent>();
	downswing_->Initialize(config_.downswingConfig);

	jumpSmash_ = std::make_unique<BossAttackJumpSmashComponent>();
	jumpSmash_->Initialize(config_.jumpSmashConfig);
}

///-------------------------------------------///
/// 更新処理
///-------------------------------------------///
BossAttackManager::UpdateResult BossAttackManager::Update(const UpdateContext& context) {
	// クールダウンを進める
	UpdateCooldowns(context.deltaTime);

	// 攻撃中でなければデフォルト値を返す
	if (currentAttack_ == AttackType::None) {
		UpdateResult result;
		result.modelRotation = context.bossRotation;
		result.isAttacking = false;
		return result;
	}

	// 実行中の攻撃を更新
	return UpdateCurrentAttack(context);
}

///-------------------------------------------///
/// ImGui情報の表示
///-------------------------------------------///
void BossAttackManager::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("攻撃マネージャー")) {
		// 現在の攻撃
		const char* attackNames[] = { "None", "Thrust", "DownwardSwing", "JumpSmash" };
		ImGui::Text("現在の攻撃: %s", attackNames[static_cast<int>(currentAttack_)]);

		ImGui::Separator();

		// クールダウン表示
		if (ImGui::TreeNode("クールダウン")) {
			ImGui::Text("Thrust:      %.2f / %.2f", cooldowns_.thrust, config_.thrustCooldown);
			ImGui::Text("DownSwing:   %.2f / %.2f", cooldowns_.downswing, config_.downswingCooldown);
			ImGui::Text("JumpSmash:   %.2f / %.2f", cooldowns_.jumpSmash, config_.jumpSmashCooldown);
			ImGui::TreePop();
		}

		// 射程設定
		if (ImGui::TreeNode("射程設定")) {
			ImGui::DragFloat("Thrust 射程", &config_.thrustRange, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("DownSwing 射程", &config_.downswingRange, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("JumpSmash 射程", &config_.jumpSmashRange, 0.1f, 0.0f, 30.0f);
			ImGui::TreePop();
		}

		// クールダウン設定
		if (ImGui::TreeNode("クールダウン設定")) {
			ImGui::DragFloat("Thrust CT", &config_.thrustCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("DownSwing CT", &config_.downswingCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("JumpSmash CT", &config_.jumpSmashCooldown, 0.1f, 0.0f, 20.0f);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// 各攻撃コンポーネントの詳細
		thrust_->Information();
		downswing_->Information();
		jumpSmash_->Information();

		ImGui::TreePop();
	}
#endif // USE_IMGUI
}

///-------------------------------------------///
/// 攻撃の選択と開始
///-------------------------------------------///
BossAttackManager::AttackType BossAttackManager::SelectAndStart(
	float distToPlayer,
	const Vector3& bossPosition,
	const Vector3& playerPosition,
	const Quaternion& bossRotation
) {
	// すでに攻撃中なら何もしない
	if (IsAttacking()) {
		return AttackType::None;
	}

	// -----------------------------------------------
	// 距離の近い順に優先して選択する。
	// クールダウンが切れていて射程内の攻撃を選ぶ。
	// Thrust → DownwardSwing → JumpSmash の順で判定し、
	// 最初にマッチした攻撃を即座に開始する。
	// -----------------------------------------------

	AttackType selected = AttackType::None;

	if (distToPlayer <= config_.thrustRange && cooldowns_.thrust <= 0.0f) {
		selected = AttackType::Thrust;
	} else if (distToPlayer <= config_.downswingRange && cooldowns_.downswing <= 0.0f) {
		selected = AttackType::DownwardSwing;
	} else if (distToPlayer <= config_.jumpSmashRange && cooldowns_.jumpSmash <= 0.0f) {
		selected = AttackType::JumpSmash;
	}

	if (selected == AttackType::None) {
		return AttackType::None;
	}

	// 選択した攻撃を開始
	currentAttack_ = selected;
	switch (selected) {
	case AttackType::Thrust:
		thrust_->StartAttack();
		break;
	case AttackType::DownwardSwing:
		downswing_->StartAttack();
		break;
	case AttackType::JumpSmash:
		jumpSmash_->StartAttack(bossPosition, playerPosition, bossRotation);
		break;
	default:
		break;
	}

	// クールダウンをセット
	SetCooldown(selected);

	return selected;
}

///-------------------------------------------///
/// 攻撃可否の確認
///-------------------------------------------///
bool BossAttackManager::IsAnyAttackAvailable(float distToPlayer) const {
	// 距離条件とクールダウン条件を両方満たす攻撃が1つでもあればtrue
	if (distToPlayer <= config_.thrustRange && cooldowns_.thrust <= 0.0f) return true;
	if (distToPlayer <= config_.downswingRange && cooldowns_.downswing <= 0.0f) return true;
	if (distToPlayer <= config_.jumpSmashRange && cooldowns_.jumpSmash <= 0.0f) return true;
	return false;
}

///-------------------------------------------///
/// 強制リセット
///-------------------------------------------///
void BossAttackManager::ForceReset() {
	thrust_->Reset();
	downswing_->Reset();
	jumpSmash_->Reset();
	currentAttack_ = AttackType::None;
}

///-------------------------------------------///
/// クールダウンのセット
///-------------------------------------------///
void BossAttackManager::SetCooldown(AttackType type) {
	switch (type) {
	case AttackType::Thrust:        cooldowns_.thrust = config_.thrustCooldown;    break;
	case AttackType::DownwardSwing: cooldowns_.downswing = config_.downswingCooldown; break;
	case AttackType::JumpSmash:     cooldowns_.jumpSmash = config_.jumpSmashCooldown; break;
	default: break;
	}
}

///-------------------------------------------///
/// クールダウンの減算
///-------------------------------------------///
void BossAttackManager::UpdateCooldowns(float deltaTime) {
	cooldowns_.thrust = std::max(0.0f, cooldowns_.thrust - deltaTime);
	cooldowns_.downswing = std::max(0.0f, cooldowns_.downswing - deltaTime);
	cooldowns_.jumpSmash = std::max(0.0f, cooldowns_.jumpSmash - deltaTime);
}

///-------------------------------------------///
/// 実行中攻撃の更新
///-------------------------------------------///
BossAttackManager::UpdateResult BossAttackManager::UpdateCurrentAttack(
	const UpdateContext& context)
{
	UpdateResult result;
	result.currentAttack = currentAttack_;
	result.isAttacking = true;

	switch (currentAttack_) {
		// -----------------------------------------------
		// Thrust（突き）
		// -----------------------------------------------
	case AttackType::Thrust:
	{
		BossAttackThrustComponent::UpdateContext ctx{
			.baseRotation = context.bossRotation,
			.deltaTime = context.deltaTime,
		};
		auto r = thrust_->Update(ctx);
		result.modelRotation = r.modelRotation;
		result.weaponLocalOffset = r.weaponLocalOffset;

		if (r.isFinished) {
			thrust_->Reset();
			currentAttack_ = AttackType::None;
			result.isAttacking = false;
			result.justFinished = true;
		}
		break;
	}
	// -----------------------------------------------
	// DownwardSwing（振り下ろし）
	// -----------------------------------------------
	case AttackType::DownwardSwing:
	{
		BossAttackDownwardSwingComponent::UpdateContext ctx{
			.baseRotation = context.bossRotation,
			.deltaTime = context.deltaTime,
		};
		auto r = downswing_->Update(ctx);
		result.modelRotation = r.rotation;
		result.weaponLocalOffset = r.weaponPosition;
		result.modelPositionDelta = r.velocity;

		if (r.isFinished) {
			downswing_->Reset();
			currentAttack_ = AttackType::None;
			result.isAttacking = false;
			result.justFinished = true;
		}
		break;
	}
	// -----------------------------------------------
	// JumpSmash（ジャンプ叩きつけ）
	// -----------------------------------------------
	case AttackType::JumpSmash:
	{
		BossAttackJumpSmashComponent::UpdateContext ctx{
			.deltaTime = context.deltaTime,
		};
		auto r = jumpSmash_->Update(ctx);
		result.modelRotation = r.rotation;
		result.weaponLocalOffset = r.weaponPosition;
		result.modelPositionDelta = r.velocity - context.bossPosition;

		if (r.isFinished) {
			jumpSmash_->Reset();
			currentAttack_ = AttackType::None;
			result.isAttacking = false;
			result.justFinished = true;
		}
		break;
	}
	default:
		result.modelRotation = context.bossRotation;
		result.isAttacking = false;
		break;
	}

	return result;
}