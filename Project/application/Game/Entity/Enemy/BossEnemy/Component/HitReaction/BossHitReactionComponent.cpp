#include "BossHitReactionComponent.h"
// C++
#include <cassert>
#include <numbers>
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
void BossHitReactionComponent::ApplyConfig(const KnockBackConfig& newConfig) {
	if (newConfig.knockBackForce < 0.0f) {
		// デバッグビルドでのみエラー
		assert(false && "Invalid KnockBackConfig");
	}
	config_ = newConfig;
}
#endif // USE_IMGUI

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void BossHitReactionComponent::Initialize(const KnockBackConfig& config) {

	// 設定の初期化
	config_ = config;

	// 状態の初期化
	state_.slowdownTimer = 0.0f;
	state_.slowdownMultiplier = 1.0f;
	state_.isHit = false;

	// 色の状態を初期化
	colorState_.colorTimer = 0.0f;
	colorState_.colorInterval = 0.0f;
	colorState_.isColorChange = false;

	// ヒット方向の初期化
	hitParentDirection_ = { 0.0f, 0.0f, 0.0f };
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
BossHitReactionComponent::UpdateResult BossHitReactionComponent::Update(const UpdateContext& context) {
	UpdateResult result{};

	/// ===タイマーの更新=== ///
	TimerUpdate(context.deltaTime);

	/// ===減速係数の更新=== ///
	UpdateSlowdownMultiplier();

	/// ===色の更新処理=== ///
	result.color = UpdateColor(context.currentColor);

	/// ===ノックバック処理=== ///
	if (state_.isHit) {
		// ノックバック方向の計算(攻撃元から離れる方向)
		Vector3 direction = hitParentDirection_;
		direction.y = 0.0f; // Y軸方向は無視

		// 正規化
		float length = Length(direction);
		if (length > 0.001f) {
			direction = Normalize(direction);
			// ノックバック速度を設定
			result.velocity = direction * config_.knockBackForce;
		}

		// ヒットフラグをリセット
		state_.isHit = false;

	} else {
		// ノックバック中でない場合は、現在の速度に減速係数を適用
		result.velocity = context.currentVelocity * state_.slowdownMultiplier;
	}

	return result;
}

///-------------------------------------------/// 
/// ImGui情報の表示
///-------------------------------------------///
void BossHitReactionComponent::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("ダメージリアクション")) {
		ImGui::DragFloat("ノックバックの強さ", &config_.knockBackForce, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("減速率", &config_.slowdownFactor, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("減速時間", &config_.slowdownDuration, 0.01f, 0.0f, 2.0f);
		ImGui::DragFloat("透明度変化時間", &config_.alphaDuration, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("ヒット時の透明度", &config_.hitAlpha, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("点滅速度", &config_.flashSpeed, 0.5f, 1.0f, 30.0f);

		ImGui::Separator();
		ImGui::Text("状態");
		ImGui::Text("減速タイマー: %.2f", state_.slowdownTimer);
		ImGui::Text("色変化タイマー: %.2f", colorState_.colorTimer);
		ImGui::Text("ヒットしたかどうか: %s", state_.isHit ? "true" : "false");
		ImGui::Text("色変化中: %s", colorState_.isColorChange ? "true" : "false");
		ImGui::TreePop();
	}
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// ヒット処理の開始
///-------------------------------------------///
void BossHitReactionComponent::OnHit(const Vector3& hitParentDirection) {
	// ヒットフラグを立てる
	state_.isHit = true;
	// 当たっている方向を取得
	hitParentDirection_ = hitParentDirection;
	// 減速タイマーを設定
	state_.slowdownTimer = config_.slowdownDuration;
	// 色変化のタイマーを設定
	colorState_.colorTimer = config_.alphaDuration;
	colorState_.colorInterval = 0.0f;
	colorState_.isColorChange = true;
}

///-------------------------------------------/// 
/// タイマーの更新処理
///-------------------------------------------///
void BossHitReactionComponent::TimerUpdate(const float deltaTime) {
	// 減速タイマーの更新
	if (state_.slowdownTimer > 0.0f) {
		state_.slowdownTimer -= deltaTime;

		if (state_.slowdownTimer < 0.0f) {
			state_.slowdownTimer = 0.0f;
		}
	}

	// 色変化タイマーの更新
	if (colorState_.colorTimer > 0.0f) {
		colorState_.colorTimer -= deltaTime;
		colorState_.colorInterval += deltaTime;

		if (colorState_.colorTimer < 0.0f) {
			colorState_.colorTimer = 0.0f;
			colorState_.colorInterval = 0.0f;
			colorState_.isColorChange = false;
		}
	}
}

///-------------------------------------------/// 
/// 減速係数の更新処理
///-------------------------------------------///
void BossHitReactionComponent::UpdateSlowdownMultiplier() {
	if (state_.slowdownTimer > 0.0f) {
		// 減速時間に応じて線形補間
		// タイマーが減るにつれて、減速率から通常速度(1.0)に戻る
		float t = state_.slowdownTimer / config_.slowdownDuration;
		state_.slowdownMultiplier = Math::Lerp(1.0f, config_.slowdownFactor, t);
	} else {
		// 減速時間が終了したら通常速度
		state_.slowdownMultiplier = 1.0f;
	}
}

///-------------------------------------------/// 
/// 色の更新処理
///-------------------------------------------///
Vector4 BossHitReactionComponent::UpdateColor(const Vector4& currentColor) const {
	Vector4 color = currentColor;

	if (colorState_.isColorChange && colorState_.colorTimer > 0.0f) {
		// サイン波を使って点滅効果を生成
		float wave = std::sin(colorState_.colorInterval * config_.flashSpeed * 2.0f * std::numbers::pi_v<float>);

		// サイン波の値が正の時は半透明、負の時は通常
		if (wave > 0.0f) {
			color.w = config_.hitAlpha;
		} else {
			color.w = 1.0f;
		}
	} else {
		// 色変化が終了したら元の色に戻す
		color.w = 1.0f;
	}

	return color;
}


