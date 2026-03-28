#include "BossAttackThrustComponent.h"
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
void BossAttackThrustComponent::ApplyConfig(const ThrustConfig& newConfig) {
	assert(newConfig.windUpDuration > 0.0f && "windUpDuration must be > 0");
	assert(newConfig.strikeDuration > 0.0f && "strikeDuration must be > 0");
	assert(newConfig.recoveryDuration > 0.0f && "recoveryDuration must be > 0");
	config_ = newConfig;
}
#endif // USE_IMGUI

///-------------------------------------------///
/// 初期化処理
///-------------------------------------------///
void BossAttackThrustComponent::Initialize(const ThrustConfig& config) {
	config_ = config;
	state_ = ThrustState{};
	phase_ = ThrustPhase::Idle;
}

///-------------------------------------------///
/// 更新処理
///-------------------------------------------///
BossAttackThrustComponent::UpdateResult BossAttackThrustComponent::Update(const UpdateContext& context) {
	UpdateResult result;
	result.isAttacking = (phase_ == ThrustPhase::Strike || phase_ == ThrustPhase::Recovery);

	// 非アクティブフェーズ：基底回転そのまま、武器は定位置
	if (phase_ == ThrustPhase::Idle || phase_ == ThrustPhase::Finished) {
		// 非アクティブ：基底回転そのまま、武器は定位置
		result.modelRotation = context.baseRotation;
		result.weaponLocalOffset = config_.weaponRestOffset;
		result.isFinished = (phase_ == ThrustPhase::Finished);
		return result;
	}

	// タイマーを進める
	state_.phaseTimer += context.deltaTime;

	// 攻撃中の更新
	UpdateAttack(context, result);
	return result;
}

///-------------------------------------------///
/// リセット
///-------------------------------------------///
void BossAttackThrustComponent::Reset() {
	state_ = ThrustState{};
	phase_ = ThrustPhase::Idle;
}

///-------------------------------------------///
/// ImGui情報の表示
///-------------------------------------------///
void BossAttackThrustComponent::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("突き攻撃情報")) {
		// 現在フェーズ
		const char* phaseNames[] = { "Idle", "WindUp", "Strike", "Recovery", "Finished" };
		ImGui::Text("フェーズ: %s", phaseNames[static_cast<int>(phase_)]);
		ImGui::Text("フェーズタイマー: %.3f", state_.phaseTimer);

		ImGui::Separator();

		if (ImGui::TreeNode("予備動作（WindUp）")) {
			ImGui::DragFloat("タメ角度 (度)", &config_.windUpAngle, 0.5f, 0.0f, 90.0f);
			ImGui::DragFloat("タメ時間 (秒)", &config_.windUpDuration, 0.01f, 0.01f, 2.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("突き（Strike）")) {
			ImGui::DragFloat("切り返し角度 (度)", &config_.strikeAngle, 0.5f, 0.0f, 90.0f);
			ImGui::DragFloat("突き時間 (秒)", &config_.strikeDuration, 0.01f, 0.01f, 2.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("戻り（Recovery）")) {
			ImGui::DragFloat("戻り時間 (秒)", &config_.recoveryDuration, 0.01f, 0.01f, 2.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("武器オフセット")) {
			ImGui::DragFloat3("定位置", &config_.weaponRestOffset.x, 0.01f);
			ImGui::DragFloat3("引き絞り", &config_.weaponWindUpOffset.x, 0.01f);
			ImGui::DragFloat3("突き出し", &config_.weaponStrikeOffset.x, 0.01f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
#endif // USE_IMGUI
}

///-------------------------------------------///
/// 攻撃開始
///-------------------------------------------///
void BossAttackThrustComponent::StartAttack() {
	// すでにアクティブな場合は無視
	if (IsActive()) {
		return;
	}
	state_.phaseTimer = 0.0f;
	phase_ = ThrustPhase::WindUp;
}

///-------------------------------------------/// 
/// 攻撃の更新を実行
///-------------------------------------------///
void BossAttackThrustComponent::UpdateAttack(const UpdateContext& context, UpdateResult& result) {
	// -----------------------------------------------
	// WindUp（予備動作）
	// -----------------------------------------------
	if (phase_ == ThrustPhase::WindUp) {
		// t
		const float t = (config_.windUpDuration > 0.0f) ? std::min(state_.phaseTimer / config_.windUpDuration, 1.0f) : 1.0f;

		// Y軸ひねり
		const Quaternion windUpYaw = MakeYawQuaternion(config_.windUpAngle * t);
		result.modelRotation = Multiply(context.baseRotation, windUpYaw);

		// 武器オフセット
		result.weaponLocalOffset = LerpVector3(
			config_.weaponRestOffset,
			config_.weaponWindUpOffset,
			t
		);

		// フェーズ遷移
		if (t >= 1.0f) {
			state_.phaseTimer = 0.0f;
			phase_ = ThrustPhase::Strike;
		}
	}
	// -----------------------------------------------
	// Strike（突き）
	// -----------------------------------------------
	else if (phase_ == ThrustPhase::Strike) {
		// t
		const float t = (config_.strikeDuration > 0.0f) ? std::min(state_.phaseTimer / config_.strikeDuration, 1.0f) : 1.0f;

		// 回転の補間
		const float startAngle = config_.windUpAngle;
		const float endAngle = -config_.strikeAngle;
		const float currentAngle = startAngle + (endAngle - startAngle) * t;
		const Quaternion strikeYaw = MakeYawQuaternion(currentAngle);
		result.modelRotation = Multiply(context.baseRotation, strikeYaw);

		// 武器オフセット
		result.weaponLocalOffset = LerpVector3(
			config_.weaponWindUpOffset,
			config_.weaponStrikeOffset,
			t
		);

		// フェーズ遷移
		if (t >= 1.0f) {
			state_.phaseTimer = 0.0f;
			phase_ = ThrustPhase::Recovery;
		}
	}
	// -----------------------------------------------
	// Recovery（戻り）
	// -----------------------------------------------
	else if (phase_ == ThrustPhase::Recovery) {
		// t
		const float t = (config_.recoveryDuration > 0.0f) ? std::min(state_.phaseTimer / config_.recoveryDuration, 1.0f) : 1.0f;

		// 回転の補間
		const Quaternion strikeEndYaw = MakeYawQuaternion(-config_.strikeAngle);
		const Quaternion strikeEndRot = Multiply(context.baseRotation, strikeEndYaw);
		result.modelRotation = Math::SLerp(strikeEndRot, context.baseRotation, t);

		// 武器オフセット
		result.weaponLocalOffset = LerpVector3(
			config_.weaponStrikeOffset,
			config_.weaponRestOffset,
			t
		);

		// フェーズ遷移
		if (t >= 1.0f) {
			phase_ = ThrustPhase::Finished;
			result.isFinished = true;
		}
	}
}

///-------------------------------------------///
/// Y軸周りに angleDeg 度回転するクォータニオンを生成
///-------------------------------------------///
Quaternion BossAttackThrustComponent::MakeYawQuaternion(float angleDeg) const {
	const float kDegToRad = Math::Pi() / 180.0f;
	const float halfRad = angleDeg * kDegToRad * 0.5f;
	return Quaternion{
		0.0f,
		std::sin(halfRad),
		0.0f,
		std::cos(halfRad)
	};
}

///-------------------------------------------///
/// Vector3 の線形補間
///-------------------------------------------///
Vector3 BossAttackThrustComponent::LerpVector3(const Vector3& a, const Vector3& b, float t) const {
	const float ct = std::clamp(t, 0.0f, 1.0f);
	return Vector3{
		a.x + (b.x - a.x) * ct,
		a.y + (b.y - a.y) * ct,
		a.z + (b.z - a.z) * ct
	};
}