#include "PlayerUI.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include <Service/GraphicsResourceGetter.h>
#include <Service/Sprite.h>
#include <Service/Camera.h>
#include <Engine/Camera/Base/CameraCommon.h>
// Math
#include <Math/MatrixMath.h>
#include <Math/Matrix4x4.h>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
PlayerUI::~PlayerUI() {
	moveUI_.reset();
	cameraUI_.reset();
	attackUI_.reset();
	avoidanceUI_.reset();
	xButton_.reset();
	aButton_.reset();
	leftStick_.reset();
	rightStick_.reset();
	hpUI_.reset();
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void PlayerUI::Initialize(Player* player, const Vector2& windowSize) {
	// Playerの保存
	player_ = player;
	
	/// ===基準スケール=== ///
	scale_ = Service::Sprite::GetBaseScale();

	// コントローラーUIのセットアップ
	SetUpControllerUI(windowSize);

	// UIの位置の保存
	moveUIPos_ = moveUI_->GetPosition();
	cameraUIPos_ = cameraUI_->GetPosition();

	/// ===HPUI=== ///
	hpUI_ = std::make_unique<HpUI>();
	// HPUIの初期化
	float maxHp = static_cast<float>(player_->GetHP());
	// HPUIのオフセットの設定
	hpUIOffset_ = { -50.0f, -150.0f };
	hpUIOffset_ *= scale_;
	// HPUIの位置の設定
	hpUIPos_ = Service::Sprite::WorldToScreen(player_->GetTransform().translate);
	// HPUIのサイズの設定
	Vector2 hpUISize = { 100.0f, 10.0f };
	hpUISize *= scale_;
	hpUI_->Initialize(hpUIPos_ + hpUIOffset_, hpUISize, maxHp);
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void PlayerUI::Update() {
	// フラグと色のリセット
	if (colorChange_.attackUI) {
		colorChange_.attackUI = false;
		attackUI_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	} else if (colorChange_.avoidanceUI) {
		colorChange_.avoidanceUI = false;
		avoidanceUI_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	}

	// スプライトの更新処理
	SpriteUpdate();

	// 色の更新処理
	ColorUpdate();

	// HPUIの更新処理
	float currentHp = static_cast<float>(player_->GetHP());
	// 座標返還後の座標を保存してHPUIに渡す
	hpUIPos_ = Service::Sprite::WorldToScreen(player_->GetTransform().translate);
	hpUI_->SetPosition(hpUIPos_ + hpUIOffset_);
	hpUI_->Update(player_->GetDeltaTime(), currentHp);
}

///-------------------------------------------/// 
/// コントローラーUIのセットアップ
///-------------------------------------------///
void PlayerUI::SetUpControllerUI(const Vector2& windowSize) {

	/// ===object2dの生成=== ///
	moveUI_ = std::make_unique<Object2d>();
	leftStick_ = std::make_unique<Object2d>();
	cameraUI_ = std::make_unique<Object2d>();
	rightStick_ = std::make_unique<Object2d>();
	attackUI_ = std::make_unique<Object2d>();
	xButton_ = std::make_unique<Object2d>();
	avoidanceUI_ = std::make_unique<Object2d>();
	aButton_ = std::make_unique<Object2d>();

	/// ===基準の設定=== ///
	// 基準点
	Vector2 point = {
		windowSize.x * (3.0f / 4.0f) - (20.0f * scale_.x),
		windowSize.y * (7.0f / 8.0f)
	};
	// 基準の間隔
	float spaceX = (windowSize.x - point.x) / 8.0f + (2.0f * scale_.x);
	// コントローラーUIのY座標
	float controllerPosY = point.y + (90.0f * scale_.y);

	// 移動UIの初期化
	moveUI_->Initialize("MoveUI");
	moveUI_->SetPosition({ point.x + spaceX, point.y });
	moveUIPos_ = moveUI_->GetPosition();
	moveUI_->SetSize({ 80.0f * scale_.x, 80.0f * scale_.y });
	moveUI_->SetAnchorPoint({ 0.5f, 0.5f });
	moveUI_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	// 左スティックUIの初期化
	leftStick_->Initialize("leftStick");
	leftStick_->SetPosition({ moveUI_->GetPosition().x, controllerPosY });
	leftStick_->SetSize({ 50.0f * scale_.x, 50.0f * scale_.y });
	leftStick_->SetAnchorPoint({ 0.5f, 0.5f });

	// カメラUIの初期化
	cameraUI_->Initialize("CameraUI");
	cameraUI_->SetPosition({ point.x + (spaceX * 3.0f), point.y });
	cameraUI_->SetSize({ 80.0f * scale_.x, 80.0f * scale_.y });
	cameraUI_->SetAnchorPoint({ 0.5f, 0.5f });
	cameraUI_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	// 右スティックUI初期化
	rightStick_->Initialize("rightStick");
	rightStick_->SetPosition({ cameraUI_->GetPosition().x, controllerPosY });
	rightStick_->SetSize({ 50.0f * scale_.x, 50.0f * scale_.y });
	rightStick_->SetAnchorPoint({ 0.5f, 0.5f });

	// 攻撃UIの初期化
	attackUI_->Initialize("AttackUI");
	attackUI_->SetPosition({ point.x + (spaceX * 5.0f), point.y });
	attackUI_->SetSize({ 80.0f * scale_.x, 80.0f * scale_.y });
	attackUI_->SetAnchorPoint({ 0.5f, 0.5f });
	attackUI_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	// XボタンUIの初期化
	xButton_->Initialize("xButton");
	xButton_->SetPosition({ attackUI_->GetPosition().x,  controllerPosY });
	xButton_->SetSize({ 50.0f * scale_.x, 50.0f * scale_.y });
	xButton_->SetAnchorPoint({ 0.5f, 0.5f });

	// 回避UIの初期化
	avoidanceUI_->Initialize("AvoidanceUI");
	avoidanceUI_->SetPosition({ point.x + (spaceX * 7.0f), point.y });
	avoidanceUI_->SetSize({ 80.0f * scale_.x, 80.0f * scale_.y });
	avoidanceUI_->SetAnchorPoint({ 0.5f, 0.5f });
	avoidanceUI_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	// AボタンUIの初期化
	aButton_->Initialize("aButton");
	aButton_->SetPosition({ avoidanceUI_->GetPosition().x, controllerPosY });
	aButton_->SetSize({ 50.0f * scale_.x, 50.0f * scale_.y });
	aButton_->SetAnchorPoint({ 0.5f, 0.5f });
	aButton_->Update();
}

///-------------------------------------------/// 
/// スプライトの更新処理
///-------------------------------------------///
void PlayerUI::SpriteUpdate() {

	/// ===スティックの取得=== ///
	Vector2 leftStick = player_->GetLeftStickState();
	Vector2 rightStick = player_->GetRightStickState();
	leftStick.y *= -1.0f;
	rightStick.y *= -1.0f;

	/// ===位置の更新=== ///
	// 移動UI
	moveUI_->SetPosition(moveUIPos_ + leftStick * 15.0f);
	// カメラUI
	cameraUI_->SetPosition(cameraUIPos_ + rightStick * 15.0f);

	/// ===サイズの管理=== ///
	if (std::abs(leftStick.x) > 0.1f || std::abs(leftStick.y) > 0.1f) {
		// サイズを大きくする
		moveUI_->SetSize({ 100.0f * scale_.x, 100.0f * scale_.y });
	} else {
		// サイズを元に戻す
		moveUI_->SetSize({ 80.0f * scale_.x, 80.0f * scale_.y });
	}
	if (std::abs(rightStick.x) > 0.1f || std::abs(rightStick.y) > 0.1f) {
		// サイズを大きくする
		cameraUI_->SetSize({ 100.0f * scale_.x, 100.0f * scale_.y });
	} else {
		// サイズを元に戻す
		cameraUI_->SetSize({ 80.0f * scale_.x, 80.0f * scale_.y });
	}
}

///-------------------------------------------/// 
/// 色の更新処理
///-------------------------------------------///
void PlayerUI::ColorUpdate() {
	/// ===色変更のフラグ管理=== ///
	if (player_->GetAttackComponent()->GetState().isActive && !colorChange_.attackUI) {
		// 攻撃UI
		colorChange_.attackUI = true;
	}
	if (player_->GetAvoidanceComponent()->GetState().isActive && !colorChange_.avoidanceUI) {
		// 回避UI
		colorChange_.avoidanceUI = true;
	}

	/// ===UIの色更新=== ///
	if (colorChange_.attackUI) {
		// 攻撃UI
		attackUI_->SetColor(activeColor_);
	}
	if (colorChange_.avoidanceUI) {
		// 回避UI
		avoidanceUI_->SetColor(activeColor_);
	}
}
