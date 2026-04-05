#include "BossAttackDownwardSwingComponent.h"
// C++
#include <cassert>
#include <algorithm>
// Math
#include <Math/sMath.h>
#include <Math/EasingMath.h>
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

#ifdef USE_IMGUI
///-------------------------------------------///
/// 設定の適用
///-------------------------------------------///
void BossAttackDownwardSwingComponent::ApplyConfig(const DownwardSwingConfig& newConfig) {
	assert(newConfig.windUpDuration > 0.0f && "windUpDuration must be > 0");
	assert(newConfig.strikeDuration > 0.0f && "strikeDuration must be > 0");
	assert(newConfig.recoveryDuration > 0.0f && "recoveryDuration must be > 0");
	config_ = newConfig;
}
#endif // USE_IMGUI

///-------------------------------------------///
/// 初期化処理
///-------------------------------------------///
void BossAttackDownwardSwingComponent::Initialize(const DownwardSwingConfig& config) {
	config_ = config;
	state_ = DownwardSwingState{};
	phase_ = DownwardSwingPhase::Idle;
}

///-------------------------------------------///
/// 更新処理
///-------------------------------------------///
BossAttackDownwardSwingComponent::UpdateResult BossAttackDownwardSwingComponent::Update(const UpdateContext& context) {
	UpdateResult result;
	result.isAttacking = (phase_ == DownwardSwingPhase::Strike || phase_ == DownwardSwingPhase::HoldDown);
	result.onStrike = false; // 毎フレーム初期化して一瞬だけtrueになるようにする

	// 非アクティブフェーズ
	if (phase_ == DownwardSwingPhase::Idle || phase_ == DownwardSwingPhase::Finished) {
		result.velocity = Vector3{ 0.0f, 0.0f, 0.0f };
		result.rotation = context.currentRotation;
		result.weaponPosition = config_.weaponRestOffset;
		result.isFinished = (phase_ == DownwardSwingPhase::Finished);
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
void BossAttackDownwardSwingComponent::Reset() {
	state_ = DownwardSwingState{};
	phase_ = DownwardSwingPhase::Idle;
}

///-------------------------------------------///
/// ImGui情報の表示
///-------------------------------------------///
void BossAttackDownwardSwingComponent::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("叩きつけ攻撃情報")) {
		const char* phaseNames[] = { "Idle", "WindUp", "Strike", "HoldDown", "Recovery", "Finished" };
		ImGui::Text("フェーズ: %s", phaseNames[static_cast<int>(phase_)]);
		ImGui::Text("フェーズタイマー: %.3f", state_.phaseTimer);

		ImGui::Separator();

		if (ImGui::TreeNode("予備動作（WindUp）")) {
			ImGui::DragFloat("仰け反り角度 (度)", &config_.windUpPitch, 0.5f, 0.0f, 60.0f);
			ImGui::DragFloat("タメ時間 (秒)", &config_.windUpDuration, 0.01f, 0.01f, 2.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("振り下ろし（Strike）")) {
			ImGui::DragFloat("前傾角度 (度)", &config_.strikeForwardPitch, 0.5f, 0.0f, 60.0f);
			ImGui::DragFloat("振り下ろし時間 (秒)", &config_.strikeDuration, 0.005f, 0.01f, 1.0f);
			ImGui::DragFloat("踏み込み距離", &config_.strikeStepForward, 0.01f, 0.0f, 3.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("余韻（HoldDown）")) {
			ImGui::DragFloat("維持時間 (秒)", &config_.holdDownDuration, 0.005f, 0.0f, 0.5f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("戻り（Recovery）")) {
			ImGui::DragFloat("戻り時間 (秒)", &config_.recoveryDuration, 0.01f, 0.01f, 2.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("武器オフセット")) {
			ImGui::DragFloat3("定位置", &config_.weaponRestOffset.x, 0.01f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
#endif // USE_IMGUI
}

///-------------------------------------------///
/// 攻撃開始
///-------------------------------------------///
void BossAttackDownwardSwingComponent::StartAttack(const Quaternion& rotation) {
	// すでにアクティブな場合は無視
	if (IsActive()) {
		return;
	}
	state_.phaseTimer = 0.0f;
	state_.baseRotation = rotation;
	phase_ = DownwardSwingPhase::WindUp;
}

///-------------------------------------------///
/// 攻撃の更新を実行
///-------------------------------------------///
void BossAttackDownwardSwingComponent::UpdateAttack(const UpdateContext& context, UpdateResult& result) {
	/// ===Phase毎の処理=== ///
	switch (phase_) {
	case DownwardSwingPhase::WindUp: UpdateWindUp(context, result); break;
	case DownwardSwingPhase::Strike: UpdateStrike(context, result); break;
	case DownwardSwingPhase::HoldDown: UpdateHoldDown(context, result); break;
	case DownwardSwingPhase::Recovery: UpdateRecovery(context, result); break;
	default: break;
	}
}

///-------------------------------------------/// 
/// WindUp（予備動作）
///-------------------------------------------///
void BossAttackDownwardSwingComponent::UpdateWindUp(const UpdateContext& context, UpdateResult& result) {
	// t
	const float rawT = (config_.windUpDuration > 0.0f) ? std::min(state_.phaseTimer / config_.windUpDuration, 1.0f) : 1.0f;
	const float t = Easing::EaseOutQuad(rawT);

	// 回転の補間
	const float currentAngle = Math::Lerp(0.0f, config_.windUpPitch, t);
	const Quaternion crouchRot = MakePitchQuaternion(currentAngle);
	result.rotation = Multiply(context.currentRotation, crouchRot);

	// 武器
	result.weaponPosition = config_.weaponRestOffset;
	// 移動なし
	result.velocity = {};

	// フェーズ遷移
	if (rawT >= 1.0f) {
		state_.phaseTimer = 0.0f;
		phase_ = DownwardSwingPhase::Strike;
	}
}

///-------------------------------------------/// 
/// Strike（振り下ろし）
///-------------------------------------------///
void BossAttackDownwardSwingComponent::UpdateStrike(const UpdateContext & context, UpdateResult & result) {
	// t
	const float rawT = (config_.strikeDuration > 0.0f) ? std::min(state_.phaseTimer / config_.strikeDuration, 1.0f) : 1.0f;
	const float t = Easing::EaseInQuad(rawT);

	// 回転の補間
	const float startAngle = config_.windUpPitch;
	const float endAngle = config_.strikeForwardPitch;
	const float currentAngle = startAngle + (endAngle - startAngle) * t;
	const Quaternion strikePitch = MakePitchQuaternion(currentAngle);
	result.rotation = Multiply(context.currentRotation, strikePitch);

	// 武器
	result.weaponPosition = config_.weaponRestOffset;

	// 踏み込み
	const float prevRawT = std::max(0.0f, (state_.phaseTimer - context.deltaTime) / config_.strikeDuration);
	const float prevT = Easing::EaseInQuad(std::min(prevRawT, 1.0f));
	const float stepTotal = config_.strikeStepForward * t;
	const float prevStepTotal = config_.strikeStepForward * prevT;
	const float deltaStep = stepTotal - prevStepTotal;

	// baseRotation でローカル前方をワールド空間へ変換
	const Vector3 localForward = { 0.0f, 0.0f, 1.0f };
	const Vector3 worldForward = Math::RotateVector(localForward, context.currentRotation);
	// 前方単位ベクトル × 今フレームの移動量
	result.velocity = Vector3{
		worldForward.x * deltaStep / context.deltaTime,
		worldForward.y * deltaStep / context.deltaTime,
		worldForward.z * deltaStep / context.deltaTime
	};

	// フェーズ遷移
	if (rawT >= 1.0f) {
		state_.phaseTimer = 0.0f;
		if (config_.holdDownDuration > 0.0f) {
			phase_ = DownwardSwingPhase::HoldDown;
		} else {
			phase_ = DownwardSwingPhase::Recovery;
		}
	}
}

///-------------------------------------------/// 
/// HoldDown（余韻）
///-------------------------------------------///
void BossAttackDownwardSwingComponent::UpdateHoldDown(const UpdateContext & context, UpdateResult & result) {
	// t
	const float t = (config_.holdDownDuration > 0.0f) ? std::min(state_.phaseTimer / config_.holdDownDuration, 1.0f) : 1.0f;

	// 終了時点の値で固定
	const Quaternion holdPitch = MakePitchQuaternion(-config_.strikeForwardPitch);
	result.rotation = Multiply(context.currentRotation, holdPitch);
	result.weaponPosition = config_.weaponRestOffset;
	result.velocity = Vector3{ 0.0f, 0.0f, 0.0f }; // 踏み込みは止まる

	// フェーズ遷移
	if (t >= 1.0f) {
		result.onStrike = true; // 波紋を出すためのトリガー
		state_.phaseTimer = 0.0f;
		phase_ = DownwardSwingPhase::Recovery;
	}
}

///-------------------------------------------/// 
/// Recovery（戻り）
///-------------------------------------------///
void BossAttackDownwardSwingComponent::UpdateRecovery(const UpdateContext& context, UpdateResult& result) {
	// t
	const float t = (config_.recoveryDuration > 0.0f) ? std::min(state_.phaseTimer / config_.recoveryDuration, 1.0f) : 1.0f;

	// 回転の補間 
	const Quaternion strikeEndRot = Multiply(context.currentRotation, MakePitchQuaternion(-config_.strikeForwardPitch));
	result.rotation = Math::SLerp(strikeEndRot, state_.baseRotation, t);

	// 武器
	result.weaponPosition = config_.weaponRestOffset;

	// Recovery中は移動しない
	result.velocity = Vector3{ 0.0f, 0.0f, 0.0f };

	// フェーズ遷移
	if (t >= 1.0f) {
		phase_ = DownwardSwingPhase::Finished;
		result.isFinished = true;
	}
}

///-------------------------------------------///
/// X軸周りに angleDeg 度回転するクォータニオンを生成
///-------------------------------------------///
Quaternion BossAttackDownwardSwingComponent::MakePitchQuaternion(float angleDeg) const {
	const float kDegToRad = Math::Pi() / 180.0f;
	const float halfRad = angleDeg * kDegToRad * 0.5f;
	return Quaternion{
		std::sin(halfRad),
		0.0f,
		0.0f,
		std::cos(halfRad)
	};
}