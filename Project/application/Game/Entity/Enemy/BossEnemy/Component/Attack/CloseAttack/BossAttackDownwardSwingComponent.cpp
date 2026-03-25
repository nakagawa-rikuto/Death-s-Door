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
	result.currentPhase = phase_;

	// 非アクティブフェーズ：基底回転そのまま、武器は定位置、速度なし
	if (phase_ == DownwardSwingPhase::Idle || phase_ == DownwardSwingPhase::Finished) {
		result.velocity = Vector3{ 0.0f, 0.0f, 0.0f };
		result.rotation = context.baseRotation;
		result.weaponPosition = config_.weaponRestOffset;
		result.isFinished = (phase_ == DownwardSwingPhase::Finished);
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
			ImGui::DragFloat3("引き上げ", &config_.weaponWindUpOffset.x, 0.01f);
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
void BossAttackDownwardSwingComponent::StartAttack() {
	// すでにアクティブな場合は無視
	if (IsActive()) {
		return;
	}
	state_.phaseTimer = 0.0f;
	phase_ = DownwardSwingPhase::WindUp;
}

///-------------------------------------------///
/// 攻撃の更新を実行
///-------------------------------------------///
void BossAttackDownwardSwingComponent::UpdateAttack(const UpdateContext& context, UpdateResult& result) {
	// -----------------------------------------------
	// WindUp（予備動作）
	//   X軸正方向へ上体を仰け反らせてタメる。
	//   EaseOut で「じわっ」と遅めに持ち上げることで
	//   次の振り下ろしとの緩急を演出する。
	//   武器は頭上の背中側へゆっくり引き上げる。
	// -----------------------------------------------
	if (phase_ == DownwardSwingPhase::WindUp) {
		const float rawT = (config_.windUpDuration > 0.0f)
			? std::min(state_.phaseTimer / config_.windUpDuration, 1.0f)
			: 1.0f;
		// EaseOut：最初は速く、終わりはじわっと
		const float t = Easing::EaseOutQuad(rawT);

		// X軸正方向（仰け反り）へ windUpPitch 度傾ける
		const Quaternion windUpPitch = MakePitchQuaternion(config_.windUpPitch * t);
		result.rotation = Multiply(context.baseRotation, windUpPitch);

		// 武器：定位置 → 頭上背中側へ
		result.weaponPosition = Math::Lerp(config_.weaponRestOffset, config_.weaponWindUpOffset, t);

		// WindUp中は踏み込みなし
		result.velocity = Vector3{ 0.0f, 0.0f, 0.0f };

		// フェーズ遷移
		if (rawT >= 1.0f) {
			state_.phaseTimer = 0.0f;
			phase_ = DownwardSwingPhase::Strike;
		}
	}
	// -----------------------------------------------
	// Strike（振り下ろし）
	//   仰け反り位置（+windUpPitch）から
	//   前傾位置（-strikeForwardPitch）まで一気に振り抜く。
	//   EaseIn で加速しながら叩きつけることで「シュッ」とした
	//   鋭さと重量感を両立させる。
	//   同時に前方へ踏み込ませてリーチと迫力を追加する。
	// -----------------------------------------------
	else if (phase_ == DownwardSwingPhase::Strike) {
		const float rawT = (config_.strikeDuration > 0.0f)
			? std::min(state_.phaseTimer / config_.strikeDuration, 1.0f)
			: 1.0f;
		// EaseIn：終盤に加速することで叩きつけの鋭さを演出
		const float t = Easing::EaseInQuad(rawT);

		// +windUpPitch → -strikeForwardPitch への補間
		const float startAngle = config_.windUpPitch;
		const float endAngle = -config_.strikeForwardPitch;
		const float currentAngle = startAngle + (endAngle - startAngle) * t;

		const Quaternion strikePitch = MakePitchQuaternion(currentAngle);
		result.rotation = Multiply(context.baseRotation, strikePitch);

		// 武器：頭上背中側 → 前方下方へ弧を描いて振り下ろす
		result.weaponPosition = Math::Lerp(config_.weaponWindUpOffset, config_.weaponStrikeOffset, t);

		// 踏み込み：前方へ加速しながら踏み込む速度を計算
		// stepZ はフレームあたりの移動量なので deltaTime で割って velocity に変換する
		const float stepZ = config_.strikeStepForward * t;
		const float prevT = Easing::EaseInQuad(std::max(0.0f,
			(state_.phaseTimer - context.deltaTime) / config_.strikeDuration));
		const float prevStepZ = config_.strikeStepForward * prevT;
		const float deltaZ = stepZ - prevStepZ;
		result.velocity = (context.deltaTime > 0.0f)
			? Vector3{ 0.0f, 0.0f, deltaZ / context.deltaTime }
		: Vector3{ 0.0f, 0.0f, 0.0f };

		// フェーズ遷移：Strike → HoldDown（または HoldDown スキップ）
		if (rawT >= 1.0f) {
			state_.phaseTimer = 0.0f;
			if (config_.holdDownDuration > 0.0f) {
				phase_ = DownwardSwingPhase::HoldDown;
			} else {
				phase_ = DownwardSwingPhase::Recovery;
			}
		}
	}
	// -----------------------------------------------
	// HoldDown（余韻）
	//   叩きつけた深い前傾姿勢を一瞬だけ維持する。
	//   武器も叩きつけ位置に止まったまま。
	//   この「止め」があることで攻撃の「出し切り感」が出る。
	// -----------------------------------------------
	else if (phase_ == DownwardSwingPhase::HoldDown) {
		const float t = (config_.holdDownDuration > 0.0f)
			? std::min(state_.phaseTimer / config_.holdDownDuration, 1.0f)
			: 1.0f;

		// 姿勢・武器・速度は Strike 終了時点の値で固定
		const Quaternion holdPitch = MakePitchQuaternion(-config_.strikeForwardPitch);
		result.rotation = Multiply(context.baseRotation, holdPitch);
		result.weaponPosition = config_.weaponStrikeOffset;
		result.velocity = Vector3{ 0.0f, 0.0f, 0.0f }; // 踏み込みは止まる

		// フェーズ遷移
		if (t >= 1.0f) {
			state_.phaseTimer = 0.0f;
			phase_ = DownwardSwingPhase::Recovery;
		}
	}
	// -----------------------------------------------
	// Recovery（戻り）
	//   -strikeForwardPitch → 0° へゆっくり戻す。
	//   武器も叩きつけ位置から定位置へ引き戻す。
	//   踏み込んだ分の位置も元へ戻す。
	//   Strike より長めの時間をかけて自然に収束させる。
	// -----------------------------------------------
	else if (phase_ == DownwardSwingPhase::Recovery) {
		const float t = (config_.recoveryDuration > 0.0f)
			? std::min(state_.phaseTimer / config_.recoveryDuration, 1.0f)
			: 1.0f;

		// -strikeForwardPitch → 0° へ補間
		const float currentAngle = -config_.strikeForwardPitch * (1.0f - t);

		const Quaternion recoveryPitch = MakePitchQuaternion(currentAngle);
		result.rotation = Multiply(context.baseRotation, recoveryPitch);

		// 武器：叩きつけ位置 → 定位置へ戻す
		result.weaponPosition = Math::Lerp(config_.weaponStrikeOffset, config_.weaponRestOffset, t);

		// Recovery中は移動しない
		result.velocity = Vector3{ 0.0f, 0.0f, 0.0f };

		// フェーズ遷移：Recovery → Finished
		if (t >= 1.0f) {
			phase_ = DownwardSwingPhase::Finished;
			result.isFinished = true;
		}
	}
}

///-------------------------------------------///
/// X軸周りに angleDeg 度回転するクォータニオンを生成
///   正値: 後方仰け反り（WindUp方向）
///   負値: 前方前傾  （Strike方向）
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