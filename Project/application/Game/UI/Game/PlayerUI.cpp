#include "PlayerUI.h"
// Player
#include "application/Game/Entity/Player/Player.h"
// Service
#include <Service/GraphicsResourceGetter.h>
#include <Service/Sprite.h>
#include <Service/Input.h>
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
	// HPUIのサイズの設定
	Vector2 hpUISize = { 300.0f, 30.0f };
	hpUISize *= scale_;
	Vector2 hpUIPosition = { windowSize.x / 12.0f - hpUISize.x / 2.0f, windowSize.y * 7.0f / 8.0f + hpUISize.y * 2.0f * scale_.y };
	// HPUIの初期化
	hpUI_ = std::make_unique<HpUI>();
	float maxHp = static_cast<float>(player_->GetHP());
	hpUI_->Initialize(hpUIPosition, hpUISize, maxHp);
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void PlayerUI::Update() {
	// フラグと色のリセット
	if (colorChange_.attackUI) {
		colorChange_.attackUI = false;
		attackUI_->SetSize(uiInfo_.size);
		attackUI_->SetColor(baseColor_);
	} 
	if (colorChange_.dodgeActiveUI) {
		colorChange_.dodgeActiveUI = false;
		dodgeUI_->SetSize(uiInfo_.size);
		dodgeUI_->SetColor(baseColor_);
	} 
	if (colorChange_.dodgeCooldownUI) {
		colorChange_.dodgeCooldownUI = false;
		dodgeUI_->SetSize(uiInfo_.size);
		dodgeUI_->SetColor(baseColor_);
	}

	// スプライトの更新処理
	SpriteUpdate();

	// 色の更新処理
	ColorUpdate();

	// HPUIの更新処理
	float currentHp = static_cast<float>(player_->GetHP());
	hpUI_->Update(player_->GetDeltaTime(), currentHp);
}

///-------------------------------------------/// 
/// コントローラーUIのセットアップ
///-------------------------------------------///
void PlayerUI::SetUpControllerUI(const Vector2& windowSize) {

	/// ===object2dの生成=== ///
	moveUI_ = std::make_unique<Object2d>();
	attackUI_ = std::make_unique<Object2d>();
	dodgeUI_ = std::make_unique<Object2d>();

	leftStick_ = std::make_unique<Object2d>();
	xButton_ = std::make_unique<Object2d>();
	aButton_ = std::make_unique<Object2d>();

	/// ===基準の設定=== ///
	// 基準点
	Vector2 point = {
		windowSize.x / 12.0f - (100.0f * scale_.x),
		windowSize.y* (7.0f / 8.0f) - 30.0f * scale_.y
	};
	// 基準の間隔
	float spaceX = point.x - 30.0f * scale_.x;
	// デバイスUIのY座標
	float devicePosY = point.y + (60.0f * scale_.y);

	// UI情報
	uiInfo_.size = { 60.0f * scale_.x, 60.0f * scale_.y };
	uiInfo_.activeSize = uiInfo_.size * 1.2f;
	uiInfo_.anchorPoint = { 0.5f, 0.5f };
	uiInfo_.color = { 0.0f, 0.0f, 0.0f, 1.0f };

	// デバイスUIのサイズ
	Vector2 deviceUISize = { 30.0f * scale_.x, 30.0f * scale_.y };

	// 移動UIの初期化
	moveUI_->Initialize("MoveUI");
	moveUI_->SetPosition({ point.x, point.y });
	moveUI_->SetSize(uiInfo_.size);
	moveUI_->SetAnchorPoint(uiInfo_.anchorPoint);
	moveUI_->SetColor(uiInfo_.color);
	moveUIPos_ = moveUI_->GetPosition();

	// 左スティックUIの初期化
	leftStick_->Initialize("leftStick");
	leftStick_->SetPosition({ moveUI_->GetPosition().x, devicePosY });
	leftStick_->SetSize(deviceUISize);
	leftStick_->SetAnchorPoint(uiInfo_.anchorPoint);

	// 攻撃UIの初期化
	attackUI_->Initialize("AttackUI");
	attackUI_->SetPosition({ point.x + (spaceX * 3.0f), point.y });
	attackUI_->SetSize(uiInfo_.size);
	attackUI_->SetAnchorPoint(uiInfo_.anchorPoint);
	attackUI_->SetColor(uiInfo_.color);

	// XボタンUIの初期化
	xButton_->Initialize("xButton");
	xButton_->SetPosition({ attackUI_->GetPosition().x,  devicePosY });
	xButton_->SetSize(deviceUISize);
	xButton_->SetAnchorPoint(uiInfo_.anchorPoint);

	// 回避UIの初期化
	dodgeUI_->Initialize("DodgeUI");
	dodgeUI_->SetPosition({ point.x + (spaceX * 6.0f), point.y });
	dodgeUI_->SetSize(uiInfo_.size);
	dodgeUI_->SetAnchorPoint(uiInfo_.anchorPoint);
	dodgeUI_->SetColor(uiInfo_.color);

	// AボタンUIの初期化
	aButton_->Initialize("aButton");
	aButton_->SetPosition({ dodgeUI_->GetPosition().x, devicePosY });
	aButton_->SetSize(deviceUISize);
	aButton_->SetAnchorPoint(uiInfo_.anchorPoint);
	aButton_->Update();
}

///-------------------------------------------/// 
/// スプライトの更新処理
///-------------------------------------------///
void PlayerUI::SpriteUpdate() {

	/// ===入力値の取得=== ///
	Vector2 leftStickValue = player_->GetLeftStickValue();
	leftStickValue.y *= -1.0f;
	Vector2 keybordValue = player_->GetKeybordValue();
	keybordValue.y *= -1.0f;

	/// ===位置の更新=== ///
	DeviceType activeDevice = Service::Input::GetActiveDevice();
	bool isValue = false;
	Vector2 moveValue = { 0.0f, 0.0f };
	// デバイスの判定
	if (activeDevice == DeviceType::Controller) {
		isValue = std::abs(leftStickValue.x) > 0.1f || std::abs(leftStickValue.y) > 0.1f;
		moveValue = leftStickValue;
	} else {
		isValue = std::abs(keybordValue.x) > 0.1f || std::abs(keybordValue.y) > 0.1f;
		moveValue = keybordValue;
	}

	// 移動UIの位置更新
	moveUI_->SetPosition(moveUIPos_ + moveValue * 5.0f);
	// 移動UIのサイズ更新
	if (isValue) {
		moveUI_->SetSize(uiInfo_.activeSize);
	} else {
		moveUI_->SetSize(uiInfo_.size);
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
		attackUI_->SetSize(uiInfo_.activeSize);
	}
	// 回避UI
	if (player_->GetActiveDodgeFlag() && !colorChange_.dodgeActiveUI) {
		colorChange_.dodgeActiveUI = true;
		dodgeUI_->SetSize(uiInfo_.activeSize);
	} else if (!player_->CanDodge() && !colorChange_.dodgeCooldownUI) {
		colorChange_.dodgeCooldownUI = true;
		dodgeUI_->SetSize(uiInfo_.size * 0.8f);
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
