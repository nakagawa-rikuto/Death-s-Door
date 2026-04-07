#include "OptionUI.h"
// Service
#include <Service/GraphicsResourceGetter.h>
#include <Service/Sprite.h>
#include "Service/Input.h"
#include "Service/Audio.h"
#ifdef USE_IMGUI
// ImGui
#include <imgui.h>
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
OptionUI::~OptionUI() {
	// スプライトのリセット
	dimSprite_.reset();
	titleButtonSprite_.reset();
	audioButtonSprite_.reset();
	audioTextSprite_.reset();
	audioSliderSprite_.reset();
	audioHandleSprite_.reset();
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void OptionUI::Initialize() {

	/// ===ウィンドウサイズの取得=== ///
	Vector2 windowSize = {
		static_cast<float>(Service::GraphicsResourceGetter::GetWindowWidth()),
		static_cast<float>(Service::GraphicsResourceGetter::GetWindowHeight())
	};

	/// ===基準スケール=== ///
	scale_ = Service::Sprite::GetBaseScale();

	// 薄暗いオーバーレイ
	dimSprite_ = std::make_unique<Object2d>();
	dimSprite_->Initialize("White");
	dimSprite_->SetBlendMode(MiiEngine::BlendMode::kBlendModeAdd);
	dimSprite_->SetPosition({ 0.0f, 0.0f });
	dimSprite_->SetSize({ windowSize.x, windowSize.y });
	dimSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 0.7f }); // 半透明の黒
	dimSprite_->SetIsDraw(false);

	/// ===AudioSprite=== ///
	audioButtonSprite_ = std::make_unique<Object2d>();
	audioButtonSprite_->Initialize("OptionVolume");
	audioButtonSprite_->SetPosition({ audioButtonPos_.x * scale_.x, audioButtonPos_.y * scale_.y });
	audioButtonSprite_->SetSize({ 200.0f * scale_.x, 50.0f * scale_.y });
	audioButtonSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	audioButtonSprite_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 黄色
	audioButtonSprite_->SetIsDraw(false);

	// オーディオスプライト
	audioHandleSprite_ = std::make_unique<Object2d>();
	audioHandleSprite_->Initialize("White");
	audioHandleSprite_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 黄色
	audioHandleSprite_->SetIsDraw(false);

	audioTextSprite_ = std::make_unique<Object2d>();
	audioTextSprite_->Initialize("White");
	audioTextSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f }); // 黒色
	audioTextSprite_->SetIsDraw(false);

	audioSliderSprite_ = std::make_unique<Object2d>();
	audioSliderSprite_->Initialize("White");
	audioSliderSprite_->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f }); // 灰色
	audioSliderSprite_->SetIsDraw(false);
	UpdateAudioSprite();

	// タイトルボタンスプライト
	titleButtonSprite_ = std::make_unique<Object2d>();
	titleButtonSprite_->Initialize("OptionTitle");
	titleButtonSprite_->SetPosition({ windowSize.x / 2.0f, windowSize.y / 2.0f + 150.0f * scale_.y });
	titleButtonSprite_->SetSize({ 400.0f * scale_.x, 150.0f * scale_.y });
	titleButtonSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	titleButtonSprite_->SetIsDraw(false);

	// 初期値設定
	returnToTitle_ = false;
	optionState_ = OptionState::Audio;
}

///-------------------------------------------/// 
/// 更新処理（タイトル）
///-------------------------------------------///
void OptionUI::TitleUpdate(bool flag) {
	if (flag) {
#ifdef USE_IMGUI

		ImGui::Begin("OptionUI");
		ImGui::Text("OptionUI");
		ImGui::DragFloat2("audioSpritePos", &audioButtonPos_.x, 1.0f);
		ImGui::DragFloat2("spritePos_", &spritePos_.x, 1.0f);
		ImGui::DragFloat2("spritePos_2_", &spritePos_2_.x, 1.0f);
		ImGui::DragFloat2("spriteSize_", &spriteSize_.x, 1.0f);
		ImGui::DragFloat2("spriteSize_2_", &spriteSize_2_.x, 1.0f);
		ImGui::End();

#endif // USE_IMGUI


		// 音量調整（左右）
		if (Service::Input::PushButton(stickNo_, ControllerButtonType::DPadRIGHT)) {
			volumeBGM_ = (std::min)(volumeBGM_ + 1, 100);
			Service::Audio::AllVolumeSound(volumeBGM_ / 100.0f);
		}
		if (Service::Input::PushButton(stickNo_, ControllerButtonType::DPadLEFT)) {
			volumeBGM_ = (std::max)(volumeBGM_ - 1, 0);
			Service::Audio::AllVolumeSound(volumeBGM_ / 100.0f);
		}

		// スプライトの更新
		UpdateAudioSprite();
	}

	// 描画
	dimSprite_->SetIsDraw(flag);
	audioButtonSprite_->SetIsDraw(flag);
	audioHandleSprite_->SetIsDraw(flag);
	audioTextSprite_->SetIsDraw(flag);
	audioSliderSprite_->SetIsDraw(flag);
	titleButtonSprite_->SetIsDraw(false);
}

///-------------------------------------------/// 
/// 更新処理（ゲーム）
///-------------------------------------------///
void OptionUI::GameUpdate(bool flag) {
	if (flag) {
		// 項目選択（上下）
		if (Service::Input::TriggerButton(stickNo_, ControllerButtonType::DPadUP) ||
			Service::Input::TriggerButton(stickNo_, ControllerButtonType::DPadDOWN) || 
			Service::Input::TriggerKey(DIK_UP) || Service::Input::TriggerKey(DIK_DOWN)) {
			// Audio <-> Title をトグル
			if (optionState_ == OptionState::Audio) {
				optionState_ = OptionState::Title;
				titleButtonSprite_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 選択中は黄色
				audioHandleSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 白色
				audioButtonSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 白色
			} else {
				optionState_ = OptionState::Audio;
				titleButtonSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 非選択中は白色
				audioHandleSprite_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 黄色
				audioButtonSprite_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 黄色
			}
		}

		// 音量調整（左右）
		if (optionState_ == OptionState::Audio) { // BGM
			if (Service::Input::PushButton(stickNo_, ControllerButtonType::DPadRIGHT)) {
				volumeBGM_ = (std::min)(volumeBGM_ + 1, 100);
				Service::Audio::AllVolumeSound(volumeBGM_ / 100.0f);
			}
			if (Service::Input::PushButton(stickNo_, ControllerButtonType::DPadLEFT)) {
				volumeBGM_ = (std::max)(volumeBGM_ - 1, 0);
				Service::Audio::AllVolumeSound(volumeBGM_ / 100.0f);
			}
		}

		// スプライトの更新
		UpdateAudioSprite();

		// タイトルに戻る
		if (optionState_ == OptionState::Title && Service::Input::TriggerButton(stickNo_, ControllerButtonType::A) || Service::Input::TriggerKey(DIK_SPACE)) {
			returnToTitle_ = true;
		}
	}
	
	// 描画するかを判別
	dimSprite_->SetIsDraw(flag);
	audioButtonSprite_->SetIsDraw(flag);
	audioHandleSprite_->SetIsDraw(flag);
	audioTextSprite_->SetIsDraw(flag);
	audioSliderSprite_->SetIsDraw(flag);
	titleButtonSprite_->SetIsDraw(flag);
}

///-------------------------------------------/// 
/// オーディオスプライトの更新
///-------------------------------------------///
void OptionUI::UpdateAudioSprite() {

	audioButtonSprite_->SetPosition({ audioButtonPos_.x * scale_.x, audioButtonPos_.y * scale_.y });

	// オーディオスプライト
	audioHandleSprite_->SetPosition({ spritePos_.x * scale_.x, spritePos_.y * scale_.y });
	audioHandleSprite_->SetAnchorPoint({ 0.0f, 0.0f });
	audioHandleSprite_->SetSize({ spriteSize_.x * scale_.x, spriteSize_.y * scale_.y });

	audioTextSprite_->SetPosition({ spritePos_2_.x * scale_.x, spritePos_2_.y * scale_.y });
	audioTextSprite_->SetAnchorPoint({ 0.0f, 0.0f });
	audioTextSprite_->SetSize({ (spriteSize_2_.x * 100.0f) * scale_.x, spriteSize_2_.y * scale_.y });

	audioSliderSprite_->SetPosition({ spritePos_2_.x * scale_.x, spritePos_2_.y * scale_.y });
	audioSliderSprite_->SetAnchorPoint({ 0.0f, 0.0f });
	audioSliderSprite_->SetSize({ (spriteSize_2_.x * static_cast<float>(volumeBGM_)) * scale_.x, spriteSize_2_.y * scale_.y });
}
