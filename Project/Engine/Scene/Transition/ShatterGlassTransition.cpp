#include "ShatterGlassTransition.h"
#include <algorithm>
// Service
#include "Service/PostEffect.h"
#include "Service/GraphicsResourceGetter.h"
// Math
#include "Math/EasingMath.h"

namespace MiiEngine {
	///-------------------------------------------/// 
	/// デストラクタ
	///-------------------------------------------///
	ShatterGlassTransition::~ShatterGlassTransition() {
		Service::PostEffect::SetOffScreenType(OffScreenType::CopyImage);
		if (sprite_) {
			sprite_.reset();
		}
	}

	///-------------------------------------------/// 
	/// フェードイン開始処理
	///-------------------------------------------///
	void ShatterGlassTransition::StartFadeIn(float duration) {
		// エフェクトタイプの設定
		Service::PostEffect::SetOffScreenType(OffScreenType::ShatterGlass);

		if (!sprite_) {
			sprite_ = std::make_unique<Object2d>();
			sprite_->Initialize("ShatterGlass");
			sprite_->SetAnchorPoint({ 0.5f, 0.5f });
			sprite_->SetPosition({ static_cast<float>(Service::GraphicsResourceGetter::GetWindowWidth()) / 2.0f, static_cast<float>(Service::GraphicsResourceGetter::GetWindowHeight()) / 2.0f });
			sprite_->SetSize({ 2000.0f, 2000.0f });
			sprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		}

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

		// 設定
		Service::PostEffect::SetShatterGlassData(data_);

		// フェードアウト開始
		SceneTransitionBase::StartFadeIn(duration);
	}

	///-------------------------------------------/// 
	/// フェードアウト開始処理
	///-------------------------------------------///
	void ShatterGlassTransition::StartFadeOut(float duration) {
		// Sprite
		sprite_ = std::make_unique<Object2d>();
		sprite_->Initialize("ShatterGlass");
		sprite_->SetAnchorPoint({ 0.5f, 0.5f });
		sprite_->SetPosition({static_cast<float>(Service::GraphicsResourceGetter::GetWindowWidth()) / 2.0f, static_cast<float>(Service::GraphicsResourceGetter::GetWindowHeight()) / 2.0f});
		sprite_->SetSize({0.0f, 0.0f});
		sprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); 

		// フェードイン開始
		SceneTransitionBase::StartFadeOut(duration);
	}

	///-------------------------------------------/// 
	/// 描画
	///-------------------------------------------///
	void ShatterGlassTransition::Draw() {}

	///-------------------------------------------/// 
	/// フェードイン固有の更新処理
	///-------------------------------------------///
	void ShatterGlassTransition::OnFadeInUpdate() {

		// 進行度を計算（0.0 ~ 1.0）
		float normalizedTime = (std::min)(animation_.currentTime / animation_.duration, 1.0f);

		// イージング適用（最初は速く、後でゆっくり）
		float easedTime = Easing::EaseOutCubic(normalizedTime);

		if (data_.progress < 0.5f) {
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
				data_.progress = 0.25f + Easing::EaseOutCubic(phaseTime) * 0.3f;
			}
		} else {
			Service::PostEffect::SetOffScreenType(OffScreenType::CopyImage);
			data_.progress = 1.0f;
			// エフェクト終了判定
			animation_.isPlaying = false;
			animation_.isFinished = true;
		}

		// 設定
		Service::PostEffect::SetShatterGlassData(data_);

		// スプライトは描画しない
		sprite_->SetIsDraw(false); 
	}

	///-------------------------------------------/// 
	/// フェードアウト固有の更新処理
	///-------------------------------------------///
	void ShatterGlassTransition::OnFadeOutUpdate() {
		// 進行度を計算（0.0 ~ 1.0）
		float normalizedTime = (std::min)(animation_.currentTime / animation_.duration, 1.0f);
		
		// イージング適用（最初は速く、後でゆっくり）
		float easedTime = Easing::EaseOutCirc(normalizedTime);
		
		// 目標サイズを計算（画面全体を覆うサイズ）
		float targetSize = Service::GraphicsResourceGetter::GetWindowWidth() * 1.5f;
		
		// イージングを適用したサイズを設定
		Vector2 spriteSize;
		spriteSize.x = easedTime * targetSize;
		spriteSize.y = easedTime * targetSize;
		
		sprite_->SetSize(spriteSize);
		sprite_->Update();
		
		// アニメーション終了判定
		if (normalizedTime >= 1.0f) {
			animation_.isPlaying = false;
			animation_.isFinished = true;
		}
	}
}