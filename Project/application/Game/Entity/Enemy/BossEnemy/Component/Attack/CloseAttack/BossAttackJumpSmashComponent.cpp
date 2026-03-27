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
	result.currentPhase = phase_;
	result.velocity = {};

	// 非アクティブフェーズ：位置・姿勢は現在値のまま返す
	if (phase_ == LeapPhase::Idle || phase_ == LeapPhase::Finished) {
		result.rotation = state_.baseRotation;
		result.weaponPosition = config_.weaponRestOffset;
		result.isFinished = (phase_ == LeapPhase::Finished);
		return result;
	}

	// タイマーを進める
	state_.phaseTimer += context.deltaTime;

	// 攻撃中の更新
	UpdateAttack(context, result);

	result.currentPhase = phase_;
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
			ImGui::DragFloat3("飛行中", &config_.weaponLeapOffset.x, 0.01f);
			ImGui::DragFloat3("叩きつけ先", &config_.weaponStrikeOffset.x, 0.01f);
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
	const Vector3& bossPosition,
	const Vector3& playerPosition,
	const Quaternion& baseRotation
) {
	// すでにアクティブな場合は無視
	if (IsActive()) {
		return;
	}
	state_.phaseTimer = 0.0f;
	state_.baseRotation = baseRotation;
	state_.startPosition = bossPosition;
	state_.targetPosition = playerPosition;
	state_.currentPosition = bossPosition;
	phase_ = LeapPhase::LeapWindUp;
}

///-------------------------------------------///
/// 攻撃の更新を実行（フェーズのディスパッチ）
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateAttack(
	const UpdateContext& context, UpdateResult& result)
{
	(void)context; // 各フェーズ関数はタイマーのみ使用

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
///   膝を曲げるイメージで前方へ少し前傾し、重心を落とす。
///   EaseOut でじわっと溜めることで跳躍の予感を演出する。
///   武器は定位置のまま。
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateLeapWindUp(UpdateResult& result) {
	const float rawT = (config_.leapWindUpDuration > 0.0f)
		? std::min(state_.phaseTimer / config_.leapWindUpDuration, 1.0f)
		: 1.0f;
	const float t = Easing::EaseOutQuad(rawT);

	// 前傾（crouchPitch は負値 → X軸負方向 = 前のめり）
	const Quaternion crouchRot = MakePitchQuaternion(config_.leapWindUpCrouchPitch * t);
	result.rotation = Multiply(state_.baseRotation, crouchRot);

	// 武器・位置は変化なし
	result.weaponPosition = config_.weaponRestOffset;
	result.velocity = {};

	// フェーズ遷移
	if (rawT >= 1.0f) {
		state_.phaseTimer = 0.0f;
		phase_ = LeapPhase::Leap;
	}
}

///-------------------------------------------///
/// Leap（放物線飛行）
///
/// 【水平移動】
///   startPosition → targetPosition を線形補間（等速）
///
/// 【垂直移動】
///   y = arcHeight * 4 * t * (1 - t)  の放物線加算
///   t=0 と t=1 でY加算=0、t=0.5 で最大高さ
///
/// 【ピッチ変化】（飛行弧への自然な追従）
///   t = 0.0〜0.5（上昇）: 0° → +ascentPitch   （前傾。前に飛び出す感）
///   t = 0.5〜1.0（降下）: +ascentPitch → -descentPitch（仰け反り→着地タメ）
///
/// 【武器】
///   定位置 → 飛行中（振り上げ）へ上昇に合わせて移動
///   降下中は振り上げ位置を維持したまま着地の瞬間を待つ
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateLeap(UpdateResult& result) {
	const float t = (config_.leapDuration > 0.0f)
		? std::min(state_.phaseTimer / config_.leapDuration, 1.0f)
		: 1.0f;

	// --- 位置計算 ---
	// 水平：線形補間
	const Vector3 horizontalPos = Math::Lerp(state_.startPosition, state_.targetPosition, t);
	// 垂直：放物線加算
	const float arcY = CalcParabolaHeight(t);
	const Vector3 nextPosition = Vector3{
		horizontalPos.x,
		horizontalPos.y + arcY,
		horizontalPos.z
	};
	// --- velocity = 今フレーム位置 - 前フレーム位置 ---
	result.velocity = nextPosition - state_.currentPosition;

	// --- ピッチ計算 ---
	float pitchAngle = 0.0f;
	if (t <= 0.5f) {
		// 上昇フェーズ：0° → +ascentPitch（前傾）
		const float ascT = t / 0.5f;              // 0.0〜1.0 に正規化
		pitchAngle = config_.leapAscentPitch * ascT;
	} else {
		// 降下フェーズ：+ascentPitch → -descentPitch（仰け反り）
		const float descT = (t - 0.5f) / 0.5f;   // 0.0〜1.0 に正規化
		const float startAngle = config_.leapAscentPitch;
		const float endAngle = -config_.leapDescentPitch;
		pitchAngle = startAngle + (endAngle - startAngle) * descT;
	}
	const Quaternion leapPitch = MakePitchQuaternion(pitchAngle);
	result.rotation = Multiply(state_.baseRotation, leapPitch);

	// --- 武器オフセット ---
	// 上昇中: 定位置 → 飛行中オフセット（振り上げ）
	// 降下中: 飛行中オフセットを維持
	if (t <= 0.5f) {
		const float weaponT = t / 0.5f;
		result.weaponPosition = Math::Lerp(config_.weaponRestOffset,config_.weaponLeapOffset,weaponT);
	} else {
		result.weaponPosition = config_.weaponLeapOffset;
	}

	// フェーズ遷移：着地（t >= 1.0）で Strike へ
	if (t >= 1.0f) {
		// 着地時は targetPosition にスナップして誤差を除去
		state_.currentPosition = state_.targetPosition;
		state_.phaseTimer = 0.0f;
		phase_ = LeapPhase::Strike;
	}
}

///-------------------------------------------///
/// Strike（着地叩きつけ）
///   Leap終了時の仰け反り（-descentPitch）を起点として
///   一気に前傾（+strikeForwardPitch）へ振り下ろす。
///   EaseIn で加速することで叩きつけの鋭さと重量感を演出する。
///   着地したので位置は targetPosition に固定。
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateStrike(UpdateResult& result) {
	const float rawT = (config_.strikeDuration > 0.0f)
		? std::min(state_.phaseTimer / config_.strikeDuration, 1.0f)
		: 1.0f;
	const float t = Easing::EaseInQuad(rawT);

	// -descentPitch → +strikeForwardPitch への補間
	const float startAngle = -config_.leapDescentPitch;
	const float endAngle = config_.strikeForwardPitch;
	const float currentAngle = startAngle + (endAngle - startAngle) * t;

	const Quaternion strikePitch = MakePitchQuaternion(currentAngle);
	result.rotation = Multiply(state_.baseRotation, strikePitch);

	// 武器：飛行中（振り上げ）→ 叩きつけ先
	result.weaponPosition = Math::Lerp(config_.weaponLeapOffset,config_.weaponStrikeOffset,t);

	// 着地済みのため移動なし
	result.velocity = {};

	// フェーズ遷移：Strike → HoldDown または Recovery
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
///   深い前傾（+strikeForwardPitch）を一瞬維持する。
///   武器も叩きつけ位置に固定。
///   この「止め」が攻撃の出し切り感・重さを演出する。
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateHoldDown(UpdateResult& result) {
	const float t = (config_.holdDownDuration > 0.0f)
		? std::min(state_.phaseTimer / config_.holdDownDuration, 1.0f)
		: 1.0f;

	// 姿勢・武器・位置は Strike 終了時点で固定
	const Quaternion holdPitch = MakePitchQuaternion(config_.strikeForwardPitch);
	result.rotation = Multiply(state_.baseRotation, holdPitch);
	result.weaponPosition = config_.weaponStrikeOffset;
	result.velocity = {};

	// フェーズ遷移
	if (t >= 1.0f) {
		state_.phaseTimer = 0.0f;
		phase_ = LeapPhase::Recovery;
	}
}

///-------------------------------------------///
/// Recovery（戻り）
///   +strikeForwardPitch → 0° へゆっくり戻す。
///   武器も叩きつけ位置から定位置へ引き戻す。
///   Strike より長めの時間で自然に収束させる。
///-------------------------------------------///
void BossAttackJumpSmashComponent::UpdateRecovery(UpdateResult& result) {
	const float t = (config_.recoveryDuration > 0.0f)
		? std::min(state_.phaseTimer / config_.recoveryDuration, 1.0f)
		: 1.0f;

	// +strikeForwardPitch → 0° へ補間
	const float currentAngle = config_.strikeForwardPitch * (1.0f - t);

	const Quaternion recoveryPitch = MakePitchQuaternion(currentAngle);
	result.rotation = Multiply(state_.baseRotation, recoveryPitch);

	// 武器：叩きつけ位置 → 定位置へ戻す
	result.weaponPosition = Math::Lerp(config_.weaponStrikeOffset,config_.weaponRestOffset,t);

	result.velocity = {};

	// フェーズ遷移：Recovery → Finished
	if (t >= 1.0f) {
		phase_ = LeapPhase::Finished;
		result.isFinished = true;
	}
}

///-------------------------------------------///
/// 放物線の高さを計算
///   y = arcHeight * 4 * t * (1 - t)
///   t=0 と t=1 で 0、t=0.5 で arcHeight になる
///-------------------------------------------///
float BossAttackJumpSmashComponent::CalcParabolaHeight(float t) const {
	const float ct = std::clamp(t, 0.0f, 1.0f);
	return config_.leapArcHeight * 4.0f * ct * (1.0f - ct);
}

///-------------------------------------------///
/// X軸周りに angleDeg 度回転するクォータニオンを生成
///   正値: 前傾（Strike方向）
///   負値: 後方仰け反り（WindUp・降下中のタメ方向）
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
