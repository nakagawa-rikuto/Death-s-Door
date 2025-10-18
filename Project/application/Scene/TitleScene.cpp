#include "TitleScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
// Service
#include "Engine/System/Service/InputService.h"
#include "Engine/System/Service/GraphicsResourceGetter.h"
#include "Engine/System/Service/OffScreenService.h"
// Math
#include "Math/sMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
TitleScene::~TitleScene() {
	// ISceneのデストラクタ
	IScene::~IScene();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void TitleScene::Initialize() {
	// ISceneの初期化(デフォルトカメラとカメラマネージャ)
	IScene::Initialize();

	// ウィンドウサイズの取得
	float windowWidth = static_cast<float>(GraphicsResourceGetter::GetWindowWidth());
	float windowHeight = static_cast<float>(GraphicsResourceGetter::GetWindowHeight());

	/// ===Transition=== ///
	transiton_ = std::make_unique<SceneTransition>();

	/// ===スプライトの初期化=== ///
	// 背景スプライト
	bgSprite_ = std::make_unique<Sprite>();
	bgSprite_->Initialize("TitleBG");
	bgSprite_->SetPosition({ 0.0f, 0.0f });
	bgSprite_->SetSize({ windowWidth, windowHeight });

	// 背景Kiriスプライト
	bgKiriSprite_ = std::make_unique<Sprite>();
	bgKiriSprite_->Initialize("TitleBGKiri");
	bgKiriSprite_->SetPosition({ 0.0f, 0.0f });
	bgKiriSprite_->SetSize({ windowWidth, windowHeight });

	// タイトルスプライト
	titleSprite_ = std::make_unique<Sprite>();
	titleSprite_->Initialize("Title"); // テクスチャファイルパスは適宜変更
	titleSprite_->SetPosition({ windowWidth / 2.0f, 150.0f });
	titleSprite_->SetSize({ 500.0f, 500.0f });
	titleSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	titleSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	// 開始スプライト
	startSprite_ = std::make_unique<Sprite>();
	startSprite_->Initialize("Start");
	startSprite_->SetPosition({ windowWidth / 2.0f, 460.0f });
	startSprite_->SetSize({ 200.0f, 50.0f });
	startSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	startSprite_->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f }); // 選択中は黄色

	// オプションスプライト
	optionSprite_ = std::make_unique<Sprite>();
	optionSprite_->Initialize("Option");
	optionSprite_->SetPosition({ windowWidth / 2.0f, 530.0f });
	optionSprite_->SetSize({ 200.0f, 50.0f });
	optionSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	optionSprite_->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f }); // 未選択は灰色

	// 終了スプライト
	exitSprite_ = std::make_unique<Sprite>();
	exitSprite_->Initialize("Exit");
	exitSprite_->SetPosition({ windowWidth / 2.0f, 600.0f });
	exitSprite_->SetSize({ 200.0f, 50.0f });
	exitSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	exitSprite_->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f });

	// 選択オーバーレイスプライト
	selectOverlay_ = std::make_unique<Sprite>();
	selectOverlay_->Initialize("OverLay"); // テクスチャファイルパスは適宜変更
	selectOverlay_->SetPosition({ windowWidth / 2.0f, 460.0f }); // 初期位置は開始の位置
	selectOverlay_->SetSize({ 400.0f, 80.0f }); // メニュー項目より少し大きめ
	selectOverlay_->SetAnchorPoint({ 0.5f, 0.5f });
	selectOverlay_->SetColor({ 0.8f, 0.8f, 0.8f, 0.8f }); // 半透明の黄色

	// 薄暗いオーバーレイ
	dimSprite_ = std::make_unique<Sprite>();
	dimSprite_->Initialize("uvChecker");
	dimSprite_->SetPosition({ 0.0f, 0.0f });
	dimSprite_->SetSize({ windowWidth, windowHeight });
	dimSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 0.7f }); // 半透明の黒

	// オプションメニュー
	optionMenuSprite_ = std::make_unique<Sprite>();
	optionMenuSprite_->Initialize("uvChecker");
	optionMenuSprite_->SetPosition({ windowWidth / 2.0f, windowHeight / 2.0f });
	optionMenuSprite_->SetSize({ 500.0f, 400.0f });
	optionMenuSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	optionMenuSprite_->SetColor({ 0.3f, 0.3f, 0.5f, 1.0f }); // 青みがかった色

	// 初期化時の位置更新
	titleSprite_->Update();
	startSprite_->Update();
	optionSprite_->Update();
	exitSprite_->Update();
	selectOverlay_->Update();
	dimSprite_->Update();
	optionMenuSprite_->Update();

	/// ===3Dモデルの初期化=== ///
	for (int i = 0; i < kModelCount; ++i) {
		models_[i] = std::make_unique<Object3d>();
		models_[i]->Init(ObjectType::Model, modelNames_[i]); // モデル名を使用

		// 初期位置設定
		if (i == currentModelIndex_) {
			// 現在選択中のモデルは中央に配置
			models_[i]->SetTranslate({ 0.0f, 0.0f, 50.0f });
		} else {
			// それ以外は画面外に配置
			models_[i]->SetTranslate({ 100.0f, 0.0f, 50.0f });
		}
	}
	models_[0]->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
	models_[1]->SetColor({ 1.0f, 0.0f, 1.0f, 1.0f });
	models_[2]->SetColor({ 0.0f, 1.0f, 1.0f, 1.0f });
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void TitleScene::Update() {
	/// ===デバック用ImGui=== ///
#ifdef USE_IMGUI
	ImGui::Begin("TitleScene");
	ImGui::Text("Current Selection: %d", static_cast<int>(currentSelection_));
	ImGui::Text("Option Open: %s", isOptionOpen_ ? "true" : "false");
	ImGui::Text("Current Model: %d", currentModelIndex_);
	ImGui::Text("Transitioning: %s", isTransitioning_ ? "true" : "false");
	ImGui::Text("Transition Timer: %.2f", transitionTimer_);
	ImGui::End();
#endif // USE_IMGUI
	
	transiton_->Update();

	/// ===スプライトの更新=== ///
	bgSprite_->Update();
	bgKiriSprite_->Update();
	titleSprite_->Update();
	startSprite_->Update();
	optionSprite_->Update();
	exitSprite_->Update();
	selectOverlay_->Update();

	/// ===モデルの更新=== ///
	if (isTransitioning_) {
		// トランジション中
		UpdateModelTransition();
	} else {
		// 通常時：現在のモデルをゆっくり回転させる
		Quaternion currentRotation = models_[currentModelIndex_]->GetTransform().rotate;
		Quaternion rotationDelta = Math::RotateY(0.03f);
		models_[currentModelIndex_]->SetRotate(Multiply(rotationDelta, currentRotation));
	}

	// 全モデルの更新
	for (int i = 0; i < kModelCount; ++i) {
		models_[i]->Update();
	}

	if (isOptionOpen_) {
		dimSprite_->Update();
		optionMenuSprite_->Update();
	}

	/// ===オプション画面の処理=== ///
	if (isOptionOpen_) {
		UpdateOptionMenu();
	} else {
		/// ===メニュー選択の更新=== ///
		UpdateMenuSelection();

		/// ===モデル選択の更新=== ///
		if (!isTransitioning_) {
			UpdateModelSelection();
		}

		/// ===決定処理=== ///
		if (InputService::TriggerButton(0, ControllerButtonType::A) || InputService::TriggerKey(DIK_SPACE)) {
			ConfirmSelection();
		}

		/// ===シーンの切り替え=== ///
		if (transiton_->GetState() == FadeState::Finished) {
			// ゲームシーンへ遷移
			sceneManager_->ChangeScene(SceneType::Game);
		}

	}

	/// ===ISceneの更新=== ///
	IScene::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void TitleScene::Draw() {
#pragma region 背景スプライト描画
	// 背景スプライト
	bgSprite_->Draw(GroundType::Back);
	//bgKiriSprite_->Draw(GroundType::Back);
	// タイトルスプライト
	titleSprite_->Draw(GroundType::Back);
#pragma endregion

#pragma region モデル描画
	// 3Dモデル（トランジション中は2つ描画）
	if (isTransitioning_) {
		models_[currentModelIndex_]->Draw(BlendMode::KBlendModeNormal);
		models_[nextModelIndex_]->Draw(BlendMode::KBlendModeNormal);
	} else {
		models_[currentModelIndex_]->Draw(BlendMode::KBlendModeNormal);
	}

	/// ===ISceneの描画=== ///
	IScene::Draw();
#pragma endregion

#pragma region 前景スプライト描画
	// メニュースプライト
	startSprite_->Draw(GroundType::Front);
	optionSprite_->Draw(GroundType::Front);
	exitSprite_->Draw(GroundType::Front);

	// 選択オーバーレイ（オプション画面が開いていない時のみ）
	if (!isOptionOpen_) {
		selectOverlay_->Draw(GroundType::Front, BlendMode::kBlendModeAdd); // 加算合成で光らせる
	}

	// オプション画面
	if (isOptionOpen_) {
		dimSprite_->Draw(GroundType::Front, BlendMode::KBlendModeNormal);
		optionMenuSprite_->Draw(GroundType::Front);
	}
#pragma endregion
}

///-------------------------------------------/// 
/// メニュー選択の更新
///-------------------------------------------///
void TitleScene::UpdateMenuSelection() {
	// 十字キー上
	if (InputService::TriggerButton(0, ControllerButtonType::DPadUP) || InputService::TriggerKey(DIK_UP)) {
		switch (currentSelection_) {
		case MenuSelection::Start:
			currentSelection_ = MenuSelection::Exit; // 一番上から一番下へ
			break;
		case MenuSelection::Option:
			currentSelection_ = MenuSelection::Start;
			break;
		case MenuSelection::Exit:
			currentSelection_ = MenuSelection::Option;
			break;
		}
		UpdateSelectOverlayPosition();
	}

	// 十字キー下
	if (InputService::TriggerButton(0, ControllerButtonType::DPadDOWN) || InputService::TriggerKey(DIK_DOWN)) {
		switch (currentSelection_) {
		case MenuSelection::Start:
			currentSelection_ = MenuSelection::Option;
			break;
		case MenuSelection::Option:
			currentSelection_ = MenuSelection::Exit;
			break;
		case MenuSelection::Exit:
			currentSelection_ = MenuSelection::Start; // 一番下から一番上へ
			break;
		}
		UpdateSelectOverlayPosition();
	}
}

///-------------------------------------------/// 
/// メニュー決定処理
///-------------------------------------------///
void TitleScene::ConfirmSelection() {
	switch (currentSelection_) {
	case MenuSelection::Start:
		OffScreenService::SetOffScreenType(OffScreenType::ShatterGlass);
		transiton_->StartFadeOut(2.0f);
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
/// メニュー項目の色更新
///-------------------------------------------///
void TitleScene::UpdateSelectOverlayPosition() {
	float windowWidth = static_cast<float>(GraphicsResourceGetter::GetWindowWidth());

	// 選択中の項目の位置に応じてオーバーレイを移動
	switch (currentSelection_) {
	case MenuSelection::Start:
		selectOverlay_->SetPosition({ windowWidth / 2.0f, 460.0f });
		break;
	case MenuSelection::Option:
		selectOverlay_->SetPosition({ windowWidth / 2.0f, 530.0f });
		break;
	case MenuSelection::Exit:
		selectOverlay_->SetPosition({ windowWidth / 2.0f, 600.0f });
		break;
	}
}

///-------------------------------------------/// 
/// オプション画面の更新
///-------------------------------------------///
void TitleScene::UpdateOptionMenu() {
	// Bボタンでオプション画面を閉じる
	if (InputService::TriggerButton(0, ControllerButtonType::B) || InputService::TriggerKey(DIK_SPACE)) {
		isOptionOpen_ = false;
	}

	// ここにオプション画面の処理を追加
}

///-------------------------------------------/// 
/// モデル選択の更新
///-------------------------------------------///
void TitleScene::UpdateModelSelection() {
	// 十字キー右：次のモデルへ
	if (InputService::TriggerButton(0, ControllerButtonType::DPadRIGHT) || InputService::TriggerKey(DIK_RIGHT)) {
		nextModelIndex_ = (currentModelIndex_ + 1) % kModelCount;
		transitionDirection_ = true; // 右方向
		isTransitioning_ = true;
		transitionTimer_ = 0.0f;

		// 次のモデルを左側の画面外に配置
		models_[nextModelIndex_]->SetTranslate({ -100.0f, 0.0f, 50.0f });
	}

	// 十字キー左：前のモデルへ
	if (InputService::TriggerButton(0, ControllerButtonType::DPadLEFT) || InputService::TriggerKey(DIK_LEFT)) {
		nextModelIndex_ = (currentModelIndex_ - 1 + kModelCount) % kModelCount;
		transitionDirection_ = false; // 左方向
		isTransitioning_ = true;
		transitionTimer_ = 0.0f;

		// 次のモデルを右側の画面外に配置
		models_[nextModelIndex_]->SetTranslate({ 100.0f, 0.0f, 50.0f });
	}
}

///-------------------------------------------/// 
/// モデルトランジションの更新
///-------------------------------------------///
void TitleScene::UpdateModelTransition() {
	// タイマー更新
	transitionTimer_ += 1.0f / 60.0f; // 60FPS想定

	// 進行度を計算（0.0～1.0）
	float progress = transitionTimer_ / transitionDuration_;
	if (progress >= 1.0f) {
		progress = 1.0f;
		isTransitioning_ = false;
		currentModelIndex_ = nextModelIndex_;
	}

	// イージング適用
	float easedProgress = EaseInOutCubic(progress);

	// 位置を補間
	if (transitionDirection_) {
		// 右へ移動
		// 現在のモデルは中央(0)から右(100)へ
		Vector3 currentPos = {
			easedProgress * 100.0f,
			0.0f,
			50.0f
		};
		models_[currentModelIndex_]->SetTranslate(currentPos);

		// 次のモデルは左(-100)から中央(0)へ
		Vector3 nextPos = {
			-100.0f + easedProgress * 100.0f,
			0.0f,
			50.0f
		};
		models_[nextModelIndex_]->SetTranslate(nextPos);
	} else {
		// 左へ移動
		// 現在のモデルは中央(0)から左(-100)へ
		Vector3 currentPos = {
			-easedProgress * 100.0f,
			0.0f,
			50.0f
		};
		models_[currentModelIndex_]->SetTranslate(currentPos);

		// 次のモデルは右(100)から中央(0)へ
		Vector3 nextPos = {
			100.0f - easedProgress * 100.0f,
			0.0f,
			50.0f
		};
		models_[nextModelIndex_]->SetTranslate(nextPos);
	}

	// 両方のモデルを回転
	Quaternion rotationDelta = Math::RotateY(0.03f);

	Quaternion currentRotation = models_[currentModelIndex_]->GetTransform().rotate;
	models_[currentModelIndex_]->SetRotate(Multiply(rotationDelta, currentRotation));

	Quaternion nextRotation = models_[nextModelIndex_]->GetTransform().rotate;
	models_[nextModelIndex_]->SetRotate(Multiply(rotationDelta, nextRotation));
}

///-------------------------------------------/// 
/// イージング関数（Ease In Out Cubic）
///-------------------------------------------///
float TitleScene::EaseInOutCubic(float t) {
	if (t < 0.5f) {
		return 4.0f * t * t * t;
	} else {
		float f = (2.0f * t - 2.0f);
		return 0.5f * f * f * f + 1.0f;
	}
}