#include "SceneTransition.h"
#include <algorithm>
// Service
#include "Engine/System/Service/OffScreenService.h"


///-------------------------------------------/// 
/// 
///-------------------------------------------///
void SceneTransition::StartFadeIn(float duration) {
	// デフォルトパラメータの設定
	data_.progress = 0.1f;
	data_.impactX = 0.5f;
	data_.impactY = 0.5f;
	data_.crackDensity = 20.0f;  // ひび割れ
	data_.dispersion = 1.2f;     // 飛散度
	data_.rotation = 1.5f;       // 回転
	data_.fadeOut = 0.0f;        // フェードアウトを有効に

	// 新しいランダムパターンを生成
	data_.randomSeed = static_cast<float>(rand() % 10000) / 10.0f;

	isPlaying_ = true;
	isFinished_ = false;
	currentTime_ = 0.0f;
	duration_ = duration;
	currentState_ = FadeState::FadeIn;
}

///-------------------------------------------/// 
/// 
///-------------------------------------------///
void SceneTransition::StartFadeOut(float duration) {
	// デフォルトパラメータの設定
	data_.progress = 0.0f;
	data_.impactX = 0.5f;
	data_.impactY = 0.5f;
	data_.crackDensity = 15.0f;
	data_.dispersion = 1.2f;
	data_.rotation = 1.5f;
	data_.fadeOut = 0.5f;  // フェードアウトを有効に

	// 新しいランダムパターンを生成
	data_.randomSeed = static_cast<float>(rand() % 10000) / 10.0f;

	isPlaying_ = true;
	isFinished_ = false;
	currentTime_ = 0.0f;
	duration_ = duration;
	currentState_ = FadeState::FadeOut;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void SceneTransition::Update() {
	if (!isPlaying_ || isFinished_) {
		return;
	}

	currentTime_ += 1.0f / 60.0f;

	// 進行度を計算（0.0 ~ 1.0）
	float normalizedTime = (std::min)(currentTime_ / duration_, 1.0f);

	// イージング適用（最初は速く、後でゆっくり）
	float easedTime = EaseOutCubic(normalizedTime);

	if (data_.progress < 0.4f) {
		// 各フェーズでの動きを調整
		if (normalizedTime < 0.5f) {
			// Phase 1: ひび割れが広がる（0.0 ~ 0.15秒）
			data_.crackDensity = 15.0f;
			data_.progress = easedTime * 0.3f;
		} else {
			// Phase 3: 破片が飛散して消える（0.5 ~ 1.0秒）
			data_.crackDensity = 15.0f;
			data_.dispersion = 4.0f;
			float phaseTime = (normalizedTime - 0.5f) / 0.5f;
			data_.progress = 0.25f + EaseOutCubic(phaseTime) * 0.3f;
		}
	} else {
		// エフェクト終了判定
		isPlaying_ = false;
		isFinished_ = true;
		currentState_ = FadeState::Finished;
		data_.progress = 1.0f;
	}

	// 設定
	OffScreenService::SetShatterGlassData(data_);
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
bool SceneTransition::IsFading() const { return isPlaying_; }
bool SceneTransition::IsFinished() const { return isFinished_; }
FadeState SceneTransition::GetState() const { return currentState_; }

///-------------------------------------------/// 
/// イージング
///-------------------------------------------///
float SceneTransition::EaseOutCubic(float t) {
	float f = t - 1.0f;
	return f * f * f + 1.0f;
}
float SceneTransition::EaseInOutQuad(float t) {
	if (t < 0.5f) {
		return 2.0f * t * t;
	} else {
		float f = 2.0f * t - 2.0f;
		return -0.5f * (f * (f - 2.0f) - 1.0f);
	}
}
