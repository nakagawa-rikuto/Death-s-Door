#include "TitleUI.h"
// Service
#include <Service/GraphicsResourceGetter.h>
#include <Service/Sprite.h>
#include "Service/Input.h"
#include "Service/DeltaTime.h"
// Math
#include "Math/EasingMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
TitleUI::~TitleUI() {
	// スプライトのリセット
	titleSprite_.reset();
	selectSprite_.start_.reset();
	selectSprite_.option_.reset();
	selectSprite_.exit_.reset();
	selectSprite_.overlay_.reset();
	// OptionUIのリセット
	optionUI_.reset();
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void TitleUI::Initialize() {
	/// ===ウィンドウサイズの取得=== ///
	Vector2 windowSize = {
		static_cast<float>(Service::GraphicsResourceGetter::GetWindowWidth()),
		static_cast<float>(Service::GraphicsResourceGetter::GetWindowHeight())
	};

	/// ===基準スケール=== ///
	Vector2 scale_ = Service::Sprite::GetBaseScale();

	/// ===基準値の設定=== ///
	spaceY_ = windowSize.y / 12.0f; // メニュー項目間の垂直スペース
	// 開始項目のY位置
	startPos_ = {
		windowSize.x / 2.0f,
		windowSize.y / 2.0f + windowSize.y / 8.0f
	};

	/// ===スロットアニメーションの初期位置設定=== ///
	float slotYOffset[selectionIndex_] = { // 初期座標
		startPos_.y - spaceY_, // 上
		startPos_.y,           // 中央
		startPos_.y + spaceY_  // 下
	};
	Vector2 slotScale[selectionIndex_] = { // 初期スケール
		sideScale_ * scale_,   // 上
		centerScale_ * scale_, // 中央
		sideScale_ * scale_    // 下
	};

	// アニメーション情報の初期化
	for (int i = 0; i < selectionIndex_; i++) {
		slotAnimation_.posY.itemCurrentY[i] = slotYOffset[i];
		slotAnimation_.posY.itemStartY[i] = slotYOffset[i];
		slotAnimation_.posY.itemTargetY[i] = slotYOffset[i];
		slotAnimation_.scale.itemCurrentScale[i] = slotScale[i];
		slotAnimation_.scale.itemStartScale[i] = slotScale[i];
		slotAnimation_.scale.itemTargetScale[i] = slotScale[i];
	}
	slotAnimation_.animationSpeed = 0.3f;
	slotAnimation_.animationTimer = 0.0f;
	slotAnimation_.isAnimating = false;

	// 初期のスロットインデックスの設定
	slotIndex_[0] = 2; // 上 = Exit
	slotIndex_[1] = 0; // 中央 = Start
	slotIndex_[2] = 1; // 下 = Option

	/// ===スプライトの初期化=== ///
	// 背景スプライト
	backgroundSprite_ = std::make_unique<Object2d>();
	backgroundSprite_->Initialize("White", MiiEngine::GroundType::Back);
	backgroundSprite_->SetSize({ 2000.0f, 2000.0f });
	backgroundSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	backgroundSprite_->Update();

	// タイトルスプライト
	titleSprite_ = std::make_unique<Object2d>();
	titleSprite_->Initialize("Title");
	titleSprite_->SetPosition({ startPos_.x, windowSize.y / 4.0f });
	titleSprite_->SetSize({ 1000.0f * scale_.x, 1000.0f * scale_.y });
	titleSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	titleSprite_->Update();

	// 開始スプライト
	selectSprite_.start_ = std::make_unique<Object2d>();
	selectSprite_.start_->Initialize("Start");
	selectSprite_.start_->SetPosition({ startPos_.x, startPos_.y });
	selectSprite_.start_->SetSize({ 200.0f * scale_.x, 50.0f * scale_.y });
	selectSprite_.start_->SetAnchorPoint({ 0.5f, 0.5f });
	selectSprite_.start_->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f }); // 未選択は灰色
	selectSprite_.start_->Update();

	// オプションスプライト
	selectSprite_.option_ = std::make_unique<Object2d>();
	selectSprite_.option_->Initialize("Option");
	selectSprite_.option_->SetPosition({ startPos_.x, startPos_.y + spaceY_ });
	selectSprite_.option_->SetSize({ 200.0f * scale_.x, 50.0f * scale_.y });
	selectSprite_.option_->SetAnchorPoint({ 0.5f, 0.5f });
	selectSprite_.option_->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f }); // 未選択は灰色
	selectSprite_.option_->Update();

	// 終了スプライト
	selectSprite_.exit_ = std::make_unique<Object2d>();
	selectSprite_.exit_->Initialize("Exit");
	selectSprite_.exit_->SetPosition({ startPos_.x, startPos_.y + spaceY_ * 2.0f });
	selectSprite_.exit_->SetSize({ 200.0f * scale_.x, 50.0f * scale_.y });
	selectSprite_.exit_->SetAnchorPoint({ 0.5f, 0.5f });
	selectSprite_.exit_->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f }); // 未選択は灰色
	selectSprite_.exit_->Update();

	// 選択オーバーレイスプライト
	selectSprite_.overlay_ = std::make_unique<Object2d>();
	selectSprite_.overlay_->Initialize("OverLay"); // テクスチャファイルパスは適宜変更
	selectSprite_.overlay_->SetPosition({ startPos_.x, startPos_.y }); // 初期位置は開始の位置
	selectSprite_.overlay_->SetSize({ 400.0f * scale_.x, 80.0f * scale_.y }); // メニュー項目より少し大きめ
	selectSprite_.overlay_->SetAnchorPoint({ 0.5f, 0.5f });
	selectSprite_.overlay_->SetColor({ 0.8f, 0.8f, 0.8f, 0.8f }); // 半透明の灰色
	selectSprite_.overlay_->Update();

	/// ===OptionUIの初期化=== ///
	optionUI_ = std::make_unique<OptionUI>();
	optionUI_->Initialize();

	/// ===フラグの初期化=== ///
	isOptionOpen_ = false;
	isGameStart_ = false;

	// スプライトの位置更新
	UpdateSlotSprites();
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void TitleUI::Update() {
	// デルタタイムの取得
	float deltaTime = Service::DeltaTime::GetDeltaTime();

	// アニメーションの更新
	UpdateAnimation(deltaTime);

	// スプライトの位置更新
	UpdateSlotSprites();

	// オプションの更新
	optionUI_->TitleUpdate(isOptionOpen_);

	// 選択状態の更新
	UpdateSelecting();
}

///-------------------------------------------/// 
/// アニメーションの更新
///-------------------------------------------///
void TitleUI::UpdateAnimation(float deltaTime) {
	if (!slotAnimation_.isAnimating) return;

	slotAnimation_.animationTimer += deltaTime;

	// tをクランプ
	float t = slotAnimation_.animationTimer / slotAnimation_.animationSpeed;
	if (t >= 1.0f) {
		t = 1.0f;
		slotAnimation_.isAnimating = false; // アニメーション終了
	}

	float easedT = Easing::EaseOutCubic(t); // 補間
	for (int i = 0; i < selectionIndex_; i++) {
		slotAnimation_.posY.itemCurrentY[i] = slotAnimation_.posY.itemStartY[i] +
			(slotAnimation_.posY.itemTargetY[i] - slotAnimation_.posY.itemStartY[i]) * easedT;

		slotAnimation_.scale.itemCurrentScale[i] = slotAnimation_.scale.itemStartScale[i] +
			(slotAnimation_.scale.itemTargetScale[i] - slotAnimation_.scale.itemStartScale[i]) * easedT;
	}
}

///-------------------------------------------/// 
/// スプライトの位置更新
///-------------------------------------------///
void TitleUI::UpdateSlotSprites() {
	// スプライトの配列（
	Object2d* sprites[selectionIndex_] = {
		selectSprite_.start_.get(),
		selectSprite_.option_.get(),
		selectSprite_.exit_.get()
	};

	for (int slot = 0; slot < selectionIndex_; slot++) {
		Object2d* sp = sprites[slotIndex_[slot]];

		sp->SetPosition({ startPos_.x, slotAnimation_.posY.itemCurrentY[slot] });
		sp->SetSize({
			200.0f * slotAnimation_.scale.itemCurrentScale[slot].x,
			 50.0f * slotAnimation_.scale.itemCurrentScale[slot].y
			});

		if (slot == 1) {
			sp->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		} else {
			sp->SetColor({ 0.6f, 0.6f, 0.6f, 0.6f });
		}
	}
}



///-------------------------------------------/// 
/// 選択状態の更新
///-------------------------------------------///
void TitleUI::UpdateSelecting() {

	/// ===オプション画面の処理=== ///
	if (isOptionOpen_) {
		UpdateOptionMenu();
	} else {
		/// ===メニュー選択の更新=== ///
		UpdateMenuSelection();

		/// ===決定処理=== ///
		if (Service::Input::TriggerButton(0, ControllerButtonType::A) || Service::Input::TriggerKey(DIK_SPACE)) {
			ConfirmSelection();
		}
	}
}

///-------------------------------------------/// 
/// メニュー選択の更新
///-------------------------------------------///
void TitleUI::UpdateMenuSelection() {

	// アニメーション中は入力を受け付けない
	if (slotAnimation_.isAnimating) return;

	bool moved = false;
	bool isUp = false; // 上キーが押されたかどうかのフラグ

	if (Service::Input::TriggerButton(0, ControllerButtonType::DPadDOWN) ||
		Service::Input::TriggerKey(DIK_W)) {
		// 上入力 → 上のアイテムが中央へ（右ローテーション）
		int tmp = slotIndex_[2];
		slotIndex_[2] = slotIndex_[1];
		slotIndex_[1] = slotIndex_[0];
		slotIndex_[0] = tmp;
		moved = true;
		isUp = true;
	}

	if (Service::Input::TriggerButton(0, ControllerButtonType::DPadUP) ||
		Service::Input::TriggerKey(DIK_S)) {
		// 下入力 → 下のアイテムが中央へ（左ローテーション）
		int tmp = slotIndex_[0];
		slotIndex_[0] = slotIndex_[1];
		slotIndex_[1] = slotIndex_[2];
		slotIndex_[2] = tmp;
		moved = true;
		isUp = false;
	}

	if (moved) {
		StartSlotAnimation(isUp);

		// currentSelection_ を中央スロットから同期
		static const MenuSelection selMap[selectionIndex_] = {
			MenuSelection::Start, MenuSelection::Option, MenuSelection::Exit
		};
		currentSelection_ = selMap[slotIndex_[1]];
	}
}

///-------------------------------------------/// 
/// スロットアニメーションの開始
///-------------------------------------------///
void TitleUI::StartSlotAnimation(bool flag) {
	// 目標Y
	float targetY[selectionIndex_] = {
		startPos_.y - spaceY_,
		startPos_.y,
		startPos_.y + spaceY_
	};
	// 目標スケール
	Vector2 targetScale[selectionIndex_] = {
		sideScale_, centerScale_, sideScale_
	};

	// ★開始Y座標をローテーション方向の逆にずらす★
   // 上キー(isUp=true)  → アイテムは下から上へ動く → 開始はひとつ下の位置
   // 下キー(isUp=false) → アイテムは上から下へ動く → 開始はひとつ上の位置
	float offset = flag ? -spaceY_ : spaceY_;

	for (int i = 0; i < selectionIndex_; i++) {
		slotAnimation_.posY.itemStartY[i] = targetY[i] + offset; // ずらした位置からスタート
		slotAnimation_.posY.itemTargetY[i] = targetY[i];
		slotAnimation_.scale.itemStartScale[i] = sideScale_;          // アニメ前は全部小さく
		slotAnimation_.scale.itemTargetScale[i] = targetScale[i];
	}

	// 中央スロットだけ開始スケールを少し大きめにする（見た目が自然になる）
	slotAnimation_.scale.itemStartScale[1] = sideScale_;

	slotAnimation_.animationTimer = 0.0f;
	slotAnimation_.isAnimating = true;
}

///-------------------------------------------/// 
/// メニュー決定処理
///-------------------------------------------///
void TitleUI::ConfirmSelection() {
	// Selectionに応じた処理
	switch (currentSelection_) {
	case MenuSelection::Start:
		// ゲーム開始処理へ移行
		isGameStart_ = true;
		break;
	case MenuSelection::Option:
		// オプション画面を開く
		isOptionOpen_ = true;
		break;
	case MenuSelection::Exit:
		// ゲーム終了
		PostQuitMessage(0);
		break;
	}
}

///-------------------------------------------/// 
/// オプション画面の更新
///-------------------------------------------///
void TitleUI::UpdateOptionMenu() {
	// Bボタンでオプション画面を閉じる
	if (Service::Input::TriggerButton(0, ControllerButtonType::B) || Service::Input::TriggerKey(DIK_SPACE)) {
		isOptionOpen_ = false;
	}
}