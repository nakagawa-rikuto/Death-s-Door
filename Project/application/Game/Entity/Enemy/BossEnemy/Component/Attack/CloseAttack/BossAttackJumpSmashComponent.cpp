#include "BossAttackJumpSmashComponent.h"
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
void BossAttackJumpSmashComponent::ApplyConfig(const JumpSmashConfig& newConfig) {
	assert(newConfig.leapWindUpDuration > 0.0f && "leapWindUpDuration must be > 0");
	assert(newConfig.leapDuration > 0.0f && "leapDuration must be > 0");
	assert(newConfig.strikeDuration > 0.0f && "strikeDuration must be > 0");
	assert(newConfig.recoveryDuration > 0.0f && "recoveryDuration must be > 0");
	config_ = newConfig;
}
#endif // USE_IMGUI

///-------------------------------------------///
/// 初期化処理
///-------------------------------------------///
void BossAttackJumpSmashComponent::Initialize(const JumpSmashConfig& config) {
	config_ = config;
	state_ = JumpSmashState{};
	phase_ = LeapPhase::Idle;
}

///-------------------------------------------///
/// 更新処理
///-------------------------------------------///
BossAttackJumpSmashComponent::UpdateResult
BossAttackJumpSmashComponent::Update(const UpdateContext& context) {
	UpdateResult result;
	result.isAttacking = (phase_ == LeapPhase::Strike || phase_ == LeapPhase::HoldDown);
	result.onJump = false;    // 毎フレーム初期化して一瞬だけtrueになるようにする
	result.onLanding = false; // 毎フレーム初期化して一瞬だけtrueになるようにする
	result.velocity = {};

	// 非アクティブフェーズ
	if (phase_ == LeapPhase::Idle || phase_ == LeapPhase::Finished) {
		result.rotation = state_.baseRotation;
		result.weaponPosition = config_.weaponRestOffset;
		result.isFinished = (phase_ == LeapPhase::Finished);
		return result;
	}

	// タイマーを進める
	state_.phaseTimer += context.deltaTime;

	// 攻撃中の更新
	UpdateAttack(result);

	return result;
}

///-------------------------------------------///
/// リセット
///-------------------------------------------///
void BossAttackJumpSmashComponent::Reset() {
	state_ = JumpSmashState{};
	phase_ = LeapPhase::Idle;
}

///-------------------------------------------///
/// ImGui情報の表示
///-------------------------------------------///
void BossAttackJumpSmashComponent::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("跳躍叩きつけ攻撃情報")) {
		const char* phaseNames[] = {
			"Idle", "LeapWindUp", "Leap", "Strike", "HoldDown", "Recovery", "Finished"
		};
		ImGui::Text("フェーズ: %s", phaseNames[static_cast<int>(phase_)]);
		ImGui::Text("フェーズタイマー: %.3f", state_.phaseTimer);
		ImGui::Text("現在位置: (%.2f, %.2f, %.2f)",
			state_.currentPosition.x, state_.currentPosition.y, state_.currentPosition.z);
		ImGui::Text("目標位置: (%.2f, %.2f, %.2f)",
			state_.targetPosition.x, state_.targetPosition.y, state_.targetPosition.z);

		ImGui::Separator();

		if (ImGui::TreeNode("跳躍前溜め（LeapWindUp）")) {
			ImGui::DragFloat("屈み前傾角度 (度)", &config_.leapWindUpCrouchPitch, 0.5f, -45.0f, 0.0f);
			ImGui::DragFloat("溜め時間 (秒)", &config_.leapWindUpDuration, 0.01f, 0.01f, 2.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("飛行（Leap）")) {
			ImGui::DragFloat("飛行時間 (秒)", &config_.leapDuration, 0.01f, 0.1f, 3.0f);
			ImGui::DragFloat("放物線頂点高さ", &config_.leapArcHeight, 0.1f, 0.5f, 20.0f);
			ImGui::DragFloat("上昇前傾角度 (度)", &config_.leapAscentPitch, 0.5f, 0.0f, 60.0f);
			ImGui::DragFloat("降下仰け反り角度(度)", &config_.leapDescentPitch, 0.5f, 0.0f, 60.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("叩きつけ（Strike）")) {
			ImGui::DragFloat("前傾角度 (度)", &config_.strikeForwardPitch, 0.5f, 0.0f, 60.0f);
			ImGui::DragFloat("叩きつけ時間 (秒)", &config_.strikeDuration, 0.005f, 0.01f, 1.0f);
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
void BossAttackJumpSmashComponent::StartAttack(
	float distance,
	const Vector3& bossPosition,
	const Vector3& playerPosition,
	const Quaternion& baseRotation
) {
	// すでにアクティブな場合は無視
	if (IsActive()) {
		return;
	}
	// 攻撃開始に必要な情報を state_ に保存
	state_.phaseTimer = 0.0f;
	state_.baseRotation = baseRotation;
	state_.startPosition = bossPosition;
	state_.targetPosition = playerPosition;
	state_.currentPosition = bossPosition;
	phase_ = LeapPhase::LeapWindUp;
	// 距離は config の範囲内にクランプして保存
	state_.distance = std::clamp(distance, config_.minDistance, config_.maxDistance);
}

///-------------------------------------------///
/// 攻撃の更新を実行（フェーズのディスパッチ）
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateAttack(UpdateResult& result) {
	/// ===Phase毎の処理=== ///
	switch (phase_) {
	case LeapPhase::LeapWindUp: UpdateLeapWindUp(result); break;
	case LeapPhase::Leap:       UpdateLeap(result);       break;
	case LeapPhase::Strike:     UpdateStrike(result);     break;
	case LeapPhase::HoldDown:   UpdateHoldDown(result);   break;
	case LeapPhase::Recovery:   UpdateRecovery(result);   break;
	default: break;
	}
}

///-------------------------------------------///
/// LeapWindUp（跳躍前の溜め）
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateLeapWindUp(UpdateResult& result) {
	// t
	const float rawT = (config_.leapWindUpDuration > 0.0f) ? std::min(state_.phaseTimer / config_.leapWindUpDuration, 1.0f) : 1.0f;
	const float t = Easing::EaseOutQuad(rawT);

	// 回転の補間
	const float crouchAngle = Math::Lerp(0.0f, config_.leapWindUpCrouchPitch, t);
	const Quaternion crouchRot = MakePitchQuaternion(crouchAngle);
	result.rotation = Multiply(state_.baseRotation, crouchRot);

	// 武器
	result.weaponPosition = config_.weaponRestOffset;
	// 移動なし
	result.velocity = {};

	// フェーズ遷移
	if (rawT >= 1.0f) {
		state_.phaseTimer = 0.0f;
		phase_ = LeapPhase::Leap;
		result.onJump = true; // 波紋を出すためのトリガー
	}
}

///-------------------------------------------///
/// Leap（放物線飛行）
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateLeap(UpdateResult& result) {
	// t
	float ration = state_.distance / config_.maxDistance;
	float actualDuration = config_.leapDuration * ration;
	const float t = (actualDuration > 0.0f) ? std::min(state_.phaseTimer / actualDuration, 1.0f) : 1.0f;

	/// ===位置計算=== ///
	// 水平
	const Vector3 horizontalPos = Math::Lerp(state_.startPosition, state_.targetPosition, t);
	// 垂直
	const float arcY = CalcParabolaHeight(t);
	const Vector3 nextPosition = Vector3{
		horizontalPos.x,
		horizontalPos.y + arcY,
		horizontalPos.z
	};
	// velocity
	result.velocity = nextPosition - state_.currentPosition;

	// currentPositionを更新更新
	state_.currentPosition = nextPosition;

	/// ===ピッチ計算=== ///
	float pitchAngle = 0.0f;
	if (t <= 0.5f) {
		// 上昇フェーズ
		const float ascT = t / 0.5f; // 0.0〜1.0 に正規化
		pitchAngle = config_.leapAscentPitch * ascT;
	} else {
		// 降下フェーズ
		const float descT = (t - 0.5f) / 0.5f; // 0.0〜1.0 に正規化
		const float startAngle = config_.leapAscentPitch;
		const float endAngle = -config_.leapDescentPitch;
		pitchAngle = startAngle + (endAngle - startAngle) * descT;
	}
	const Quaternion leapPitch = MakePitchQuaternion(pitchAngle);
	result.rotation = Multiply(state_.baseRotation, leapPitch);

	/// ===武器オフセット=== ///
	if (t <= 0.5f) { // 上昇中
		const float weaponT = t / 0.5f;
		result.weaponPosition = Math::Lerp(config_.weaponRestOffset, config_.weaponRestOffset, weaponT);
	} else { // 降下中
		result.weaponPosition = config_.weaponRestOffset;
	}

	// フェーズ遷移
	if (t >= 1.0f) {
		// 着地時は targetPosition にスナップして誤差を除去
		state_.currentPosition = state_.targetPosition;
		state_.phaseTimer = 0.0f;
		phase_ = LeapPhase::Strike;
		result.onLanding = true; // 波紋を出すためのトリガー
		result.onStrike = true;
	}
}

///-------------------------------------------///
/// Strike（着地叩きつけ）
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateStrike(UpdateResult& result) {

	// t
	const float rawT = (config_.strikeDuration > 0.0f) ? std::min(state_.phaseTimer / config_.strikeDuration, 1.0f) : 1.0f;
	const float t = Easing::EaseInQuad(rawT);

	// 回転の補間
	const float startAngle = -config_.leapDescentPitch;
	const float endAngle = config_.strikeForwardPitch;
	const float currentAngle = startAngle + (endAngle - startAngle) * t;
	const Quaternion strikePitch = MakePitchQuaternion(currentAngle);
	result.rotation = Multiply(state_.baseRotation, strikePitch);

	// 武器
	result.weaponPosition = Math::Lerp(config_.weaponRestOffset, config_.weaponRestOffset, t);

	// 着地済みのため移動なし
	result.velocity = {};

	// フェーズ遷移
	if (rawT >= 1.0f) {
		state_.phaseTimer = 0.0f;
		if (config_.holdDownDuration > 0.0f) {
			phase_ = LeapPhase::HoldDown;
		} else {
			phase_ = LeapPhase::Recovery;
		}
	}
}

///-------------------------------------------///
/// HoldDown（叩きつけ余韻）
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateHoldDown(UpdateResult& result) {
	// t
	const float t = (config_.holdDownDuration > 0.0f) ? std::min(state_.phaseTimer / config_.holdDownDuration, 1.0f) : 1.0f;

	// 終了時点で固定
	const Quaternion holdPitch = MakePitchQuaternion(config_.strikeForwardPitch);
	result.rotation = Multiply(state_.baseRotation, holdPitch);
	result.weaponPosition = config_.weaponRestOffset;
	result.velocity = {};

	// フェーズ遷移
	if (t >= 1.0f) {
		state_.phaseTimer = 0.0f;
		phase_ = LeapPhase::Recovery;
	}
}

///-------------------------------------------///
/// Recovery（戻り）
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateRecovery(UpdateResult& result) {
	// t
	const float t = (config_.recoveryDuration > 0.0f) ? std::min(state_.phaseTimer / config_.recoveryDuration, 1.0f) : 1.0f;

	// 回転の補間
	const Quaternion strikeEndRot = Multiply(state_.baseRotation, MakePitchQuaternion(config_.strikeForwardPitch));
	result.rotation = Math::SLerp(strikeEndRot, state_.baseRotation, t);

	// 武器
	result.weaponPosition = Math::Lerp(config_.weaponRestOffset, config_.weaponRestOffset, t);

	// velocity
	result.velocity = {};

	// フェーズ遷移
	if (t >= 1.0f) {
		phase_ = LeapPhase::Finished;
		result.isFinished = true;
	}
}

///-------------------------------------------///
/// 放物線の高さを計算
///-------------------------------------------///
float BossAttackJumpSmashComponent::CalcParabolaHeight(float t) const {
	const float ct = std::clamp(t, 0.0f, 1.0f);
	return config_.leapArcHeight * 4.0f * ct * (1.0f - ct);
}

///-------------------------------------------///
/// X軸周りに angleDeg 度回転するクォータニオンを生成
///-------------------------------------------///
Quaternion BossAttackJumpSmashComponent::MakePitchQuaternion(float angleDeg) const {
	const float kDegToRad = Math::Pi() / 180.0f;
	const float halfRad = angleDeg * kDegToRad * 0.5f;
	return Quaternion{
		std::sin(halfRad),
		0.0f,
		0.0f,
		std::cos(halfRad)
	};
}