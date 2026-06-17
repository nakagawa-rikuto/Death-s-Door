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
	attackUI_.reset();
	dodgeUI_.reset();
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
		attackUI_->SetColor(baseColor_);
	} 
	if (colorChange_.dodgeActiveUI) {
		colorChange_.dodgeActiveUI = false;
		dodgeUI_->SetColor(baseColor_);
	} 
	if (colorChange_.dodgeCooldownUI) {
		colorChange_.dodgeCooldownUI = false;
		dodgeUI_->SetColor(baseColor_);
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
	rightStick_ = std::make_unique<Object2d>();
	attackUI_ = std::make_unique<Object2d>();
	xButton_ = std::make_unique<Object2d>();
	dodgeUI_ = std::make_unique<Object2d>();
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

	// 攻撃UIの初期化
	attackUI_->Initialize("AttackUI");
	attackUI_->SetPosition({ point.x + (spaceX * 3.0f), point.y });
	attackUI_->SetSize({ 80.0f * scale_.x, 80.0f * scale_.y });
	attackUI_->SetAnchorPoint({ 0.5f, 0.5f });
	attackUI_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	// XボタンUIの初期化
	xButton_->Initialize("xButton");
	xButton_->SetPosition({ attackUI_->GetPosition().x,  controllerPosY });
	xButton_->SetSize({ 50.0f * scale_.x, 50.0f * scale_.y });
	xButton_->SetAnchorPoint({ 0.5f, 0.5f });

	// 回避UIの初期化
	dodgeUI_->Initialize("DodgeUI");
	dodgeUI_->SetPosition({ point.x + (spaceX * 5.0f), point.y });
	dodgeUI_->SetSize({ 80.0f * scale_.x, 80.0f * scale_.y });
	dodgeUI_->SetAnchorPoint({ 0.5f, 0.5f });
	dodgeUI_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	// AボタンUIの初期化
	aButton_->Initialize("aButton");
	aButton_->SetPosition({ dodgeUI_->GetPosition().x, controllerPosY });
	aButton_->SetSize({ 50.0f * scale_.x, 50.0f * scale_.y });
	aButton_->SetAnchorPoint({ 0.5f, 0.5f });
	aButton_->Update();
}

///-------------------------------------------/// 
/// スプライトの更新処理
///-------------------------------------------///
void PlayerUI::SpriteUpdate() {

	/// ===スティックの取得=== ///
	Vector2 leftStick = player_->GetLeftStickValue();
	leftStick.y *= -1.0f;

	/// ===位置の更新=== ///
	// 移動UI
	moveUI_->SetPosition(moveUIPos_ + leftStick * 15.0f);

	/// ===サイズの管理=== ///
	if (std::abs(leftStick.x) > 0.1f || std::abs(leftStick.y) > 0.1f) {
		// サイズを大きくする
		moveUI_->SetSize({ 100.0f * scale_.x, 100.0f * scale_.y });
	} else {
		// サイズを元に戻す
		moveUI_->SetSize({ 80.0f * scale_.x, 80.0f * scale_.y });
	}
}

///-------------------------------------------/// 
/// 色の更新処理
///-------------------------------------------///
void PlayerUI::ColorUpdate() {
	/// ===色変更のフラグ管理=== ///
	// 攻撃UI
	if (player_->GetActiveAttackFlag() && !colorChange_.attackUI) {
		colorChange_.attackUI = true;
	}
	// 回避UI
	if (player_->GetActiveDodgeFlag() && !colorChange_.dodgeActiveUI) {
		colorChange_.dodgeActiveUI = true;
	} else if (!player_->CanDodge() && !colorChange_.dodgeCooldownUI) {
		colorChange_.dodgeCooldownUI = true;
	}

	/// ===UIの色更新=== ///
	if (colorChange_.attackUI) {
		// 攻撃UI
		attackUI_->SetColor(activeColor_);
	}
	if (colorChange_.dodgeActiveUI) {
		// 回避UI
		dodgeUI_->SetColor(activeColor_);
	} 
	if (colorChange_.dodgeCooldownUI) {
		// 回避UI
		dodgeUI_->SetColor(cooldownColor_);
	}
}
