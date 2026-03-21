#include "EnemyTeleportComponent.h"
// C++
#include <cassert>
// Math
#include <Math/sMath.h>
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI


///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void EnemyTeleportComponent::Initialize(const TeleportConfig& config) {
	// ランダムエンジンの初期化
	std::seed_seq seed{
		static_cast<uint32_t>(std::time(nullptr)),
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(this)),
	};
	randomEngine_.seed(seed);

	// 設定の初期化
	config_ = config;
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
EnemyTeleportComponent::UpdateResult EnemyTeleportComponent::Update(const UpdateContext & context) {

	UpdateResult result{};

	/// ===フェーズタイマーと総経過時間を進める=== ///
	state_.phaseTimer += context.deltaTime;

	// テレポート中
	UpdateTeleport(context, result);

	// 返す
	return result;
}

///-------------------------------------------/// 
/// ImGui情報の表示
///-------------------------------------------///
void EnemyTeleportComponent::Information() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("テレポート情報")) {
		ImGui::DragFloat("SpinOut時間", &config_.spinOutDuration, 0.01f, 0.05f, 2.0f);
		ImGui::DragFloat("Warp時間", &config_.warpDuration, 0.01f, 0.01f, 1.0f);
		ImGui::DragFloat("SpinIn時間", &config_.spinInDuration, 0.01f, 0.05f, 2.0f);
		ImGui::Separator();
		const char* phaseNames[] = { "None", "SpinOut", "FadeOut", "FadeIn", "SpinIn" };
		ImGui::Text("Phase   : %s", phaseNames[static_cast<int>(state_.phase)]);
		ImGui::Text("Alpha   : %.2f", state_.alpha);
		ImGui::TreePop();
	}
#endif // USE_IMGUI
}

///-------------------------------------------/// 
/// テレポートの開始
///-------------------------------------------///
void EnemyTeleportComponent::Start(
	const Vector3& currentPosition,
	const Quaternion& currentRotation,
	const Vector3& playerPosition,
	float minRange, 
	float maxRange) {

	// 状態のリセット
	state_.phase = Phase::SpinOut;
	state_.phaseTimer = 0.0f;
	state_.alpha = 1.0f;
	state_.startRotation = currentRotation;
	state_.rotation = state_.startRotation;

	// ワープ先を決定
	Vector3 offset = SetNextPosition(minRange, maxRange, currentPosition);
	state_.nextPosition = offset + playerPosition;  // 現在位置からのオフセットとして計算
	state_.nextPosition.y = currentPosition.y;		// Y座標は変えない
}

///-------------------------------------------/// 
/// ワープ先の座標を設定
///-------------------------------------------///
Vector3 EnemyTeleportComponent::SetNextPosition(float minRange, float maxRange, const Vector3& currentPosition) {
	
	/// ===計算=== ///
	// minRange と maxRange の大小関係を保証する
	if (minRange > maxRange) {
		std::swap(minRange, maxRange);
	}

	// ランダムな角度と距離を生成
	std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * Math::Pi());
	std::uniform_real_distribution<float> distanceDist(minRange, maxRange);

	// ランダムな値の設定
	float angle = angleDist(randomEngine_);
	float distance = distanceDist(randomEngine_);

	// 移動先の設定
	return Vector3{
		std::cos(angle) * distance,
		currentPosition.y,
		std::sin(angle) * distance
	};
}

///-------------------------------------------/// 
/// テレポート演出の更新処理\
///-------------------------------------------///
void EnemyTeleportComponent::UpdateTeleport(const UpdateContext& context, UpdateResult& result) {

	/// ===現在位置を演出時間内に均等に移動させる=== ///
	/*const float remaining = state_.totalTime - std::min(state_.elapsed, state_.totalTime);
	if (remaining > 0.0f) {
		Vector3 toNext = state_.nextPosition - context.currentPosition;
		toNext.y = 0.0f;  // Y は外部（重力など）に委ねる
		result.velocity = toNext / remaining;
	} else {
		result.velocity = { 0.0f, 0.0f, 0.0f };
	}*/

	/// ===フェーズごとの演出=== ///
	switch (state_.phase) {
	case Phase::SpinOut: {// SpinOut：消える前の一周回転
		const float duration = config_.spinOutDuration;
		float t = (duration > 0.0f) ? (state_.phaseTimer / duration) : 1.0f;
		t = std::min(t, 1.0f);

		// 透明度を線形に下げる（1 → 0）
		state_.alpha = 1.0f - t;
		result.colorAlpha = state_.alpha;

		// t² の EaseIn で 1周（2π）回転（startRotation を基点に乗算）
		const float spinAngle = (t * t) * 2.0f * Math::Pi();
		state_.rotation = Multiply(state_.startRotation, MakeRotationY(spinAngle));
		result.rotation = state_.rotation;

		// 移動なし
		result.velocity = { 0.0f, 0.0f, 0.0f };

		// 現在地にパーティクルを出し続ける
		result.shouldSpawnParticleAtCurrent = true;

		if (t >= 1.0f) {
			// 完全に透明になったら Warp フェーズへ
			result.shouldSpawnParticleAtCurrent = false;
			state_.startRotation = context.currentRotation;
			state_.phase = Phase::Warp;
			state_.phaseTimer = 0.0f;
		}
		break;
	}
	case Phase::Warp: { // Warp：完全透明のまま座標をワープ
		const float duration = config_.warpDuration;

		// 完全に透明のまま
		state_.alpha = 0.0f;
		result.colorAlpha = 0.0f;
		result.rotation = context.currentRotation;

		// warpStartPosition → nextPosition を warpDuration 秒で均等に移動
		const float remaining = duration - std::min(state_.phaseTimer, duration);
		if (remaining > 0.0f) {
			Vector3 toNext = state_.nextPosition - context.currentPosition;
			toNext.y = 0.0f;  
			result.velocity = toNext * remaining;
		} else {
			result.velocity = { 0.0f, 0.0f, 0.0f };
			// NextPosition に到達したら SpinIn へ
			state_.startRotation = context.currentRotation;
			state_.phase = Phase::SpinIn;
			state_.phaseTimer = 0.0f;

			// 到達タイミングで出現先パーティクルを1度出す
			result.shouldSpawnParticleAtNext = true;
		}
		break;
	}
	case Phase::SpinIn: {// SpinIn：出現後の一周回転（EaseOut＝減速）
		const float duration = config_.spinInDuration;
		float t = (duration > 0.0f) ? (state_.phaseTimer / duration) : 1.0f;
		t = std::min(t, 1.0f);

		// 透明度を線形に上げる（0 → 1）
		state_.alpha = t;
		result.colorAlpha = state_.alpha;

		// √t の EaseOut で 1周（2π）回転（startRotation を基点に乗算）
		const float spinAngle = std::sqrt(t) * 2.0f * Math::Pi();
		state_.rotation = Multiply(state_.startRotation, MakeRotationY(spinAngle));
		result.rotation = state_.rotation;

		// 移動なし
		result.velocity = { 0.0f, 0.0f, 0.0f };

		// 出現先にパーティクルを出し続ける
		result.shouldSpawnParticleAtNext = true;

		if (t >= 1.0f) {
			// 演出完了
			result.shouldSpawnParticleAtNext = false;
			result.shouldResetRotationFlag = true;
			result.isComplete = true;
			state_.phase = Phase::None;
		}
		break;
	}
	default:
		break;
	}
}

///-------------------------------------------/// 
/// Y軸回転のクォータニオンを作成
///-------------------------------------------///
Quaternion EnemyTeleportComponent::MakeRotationY(float angleRad) {
	const float half = angleRad * 0.5f;
	return Quaternion{
		0.0f,
		std::sin(half),
		0.0f,
		std::cos(half)
	};
}
