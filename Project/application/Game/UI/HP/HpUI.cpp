#include "HpUI.h"
// Service
#include <Service/Sprite.h>
// Math
#include <algorithm>
#include <cmath>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
HpUI::~HpUI() {
	hpBackground_.reset();
	hpDamage_.reset();
	hpBar_.reset();
	hpFrame_.reset();
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void HpUI::Initialize(const Vector2& position, const Vector2& size, float maxHP) {
	position_ = position;
	size_ = size;

	/// ===基準スケールの取得=== ///
	scale_ = Service::Sprite::GetBaseScale();

	/// ===枠=== ///
	hpFrame_ = std::make_unique<Object2d>();
	hpFrame_->Initialize("White");
	hpFrame_->SetPosition(position_);
	hpFrame_->SetSize(size_);
	hpFrame_->SetAnchorPoint({ 0.0f, 0.0f });

	/// ===背景（黒）=== ///
	// HPがゼロになったときに見える黒背景
	hpBackground_ = std::make_unique<Object2d>();
	hpBackground_->Initialize("White");  // 単色スプライト
	hpBackground_->SetPosition(position_);
	hpBackground_->SetSize(size_);
	hpBackground_->SetAnchorPoint({ 0.0f, 0.0f });
	hpBackground_->SetColor({ 0.1f, 0.1f, 0.1f, 1.0f });

	/// ===ダメージ遅延バー（赤）=== ///
	// ダメージを受けた後、一定時間赤く表示される
	hpDamage_ = std::make_unique<Object2d>();
	hpDamage_->Initialize("White");
	hpDamage_->SetPosition(position_);
	hpDamage_->SetSize(size_);
	hpDamage_->SetAnchorPoint({ 0.0f, 0.0f });
	hpDamage_->SetColor({ 0.9f, 0.15f, 0.15f, 1.0f });

	/// ===現在HPバー（緑）=== ///
	// 実際の現在HPをリアルタイムで表示する
	hpBar_ = std::make_unique<Object2d>();
	hpBar_->Initialize("White");
	hpBar_->SetPosition(position_);
	hpBar_->SetSize(size_);
	hpBar_->SetAnchorPoint({ 0.0f, 0.0f });
	hpBar_->SetColor({ 0.1f, 0.85f, 0.2f, 1.0f });

	

	/// ===初期状態（HP満タン）=== ///
	displayHP_ = 1.0f;
	damageHP_ = 1.0f;
	previousHP_ = 1.0f;
	maxHP_ = maxHP;
	isDamageVisible_ = false;
	damageTimer_ = 0.0f;

	UpdateSprites();
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void HpUI::Update(float deltaTime, float currentHP) {

	/// ===HP割合の計算=== ///
	const float ratio = (maxHP_ > 0.0f) ? std::clamp(currentHP / maxHP_, 0.0f, 1.0f) : 0.0f;

	/// ===ダメージ検出=== ///
	// 前フレームよりHPが下がっていればダメージとみなす
	if (ratio < previousHP_) {
		// 赤バーは現在表示されている緑バー（displayHP_）の位置で保持し、タイマーをリセット
		if (!isDamageVisible_) {
			damageHP_ = displayHP_;
		}
		damageTimer_ = 0.0f;
		isDamageVisible_ = true;
	}
	previousHP_ = ratio;

	/// ===現在HPをイージングでスムーズに反映=== ///
	float easeT = easingSpeed_ * deltaTime;
	if (easeT > 1.0f) { easeT = 1.0f; }
	displayHP_ += (ratio - displayHP_) * easeT;

	/// ===ダメージ遅延処理=== ///
	if (isDamageVisible_) {
		damageTimer_ += deltaTime;

		if (damageTimer_ >= damageDelay_) {
			// 一定時間が経つと赤バーがイージングで速く緑バーに追いつく
			float redEaseT = redEasingSpeed_ * deltaTime;
			if (redEaseT > 1.0f) { redEaseT = 1.0f; }
			damageHP_ += (displayHP_ - damageHP_) * redEaseT;

			// 十分に近づいたら完全に合わせて非表示にする
			if (damageHP_ - displayHP_ <= 0.001f) {
				damageHP_ = displayHP_;
				isDamageVisible_ = false;
			}
		}
	} else {
		// ダメージ非表示時は赤バーも緑バーに追従させる
		damageHP_ = displayHP_;
	}

	/// ===スプライトへ反映=== ///
	UpdateSprites();
}

///-------------------------------------------/// 
/// スプライトの位置・サイズを更新する
///-------------------------------------------///
void HpUI::UpdateSprites() {
	/// ===背景は常にフルサイズ=== ///
	hpBackground_->SetSize(size_);
	hpBackground_->Update();

	/// ===赤バー（ダメージ遅延）=== ///
	// damageHP_ 割合分だけ横幅を設定する
	Vector2 damageSize = { size_.x * damageHP_, size_.y };
	hpDamage_->SetSize(damageSize);
	hpDamage_->Update();

	/// ===緑バー（現在HP）=== ///
	// displayHP_ 割合分だけ横幅を設定する
	Vector2 barSize = { size_.x * displayHP_, size_.y };
	hpBar_->SetSize(barSize);
	hpBar_->Update();

	/// ===枠は常にフルサイズ=== ///
	hpFrame_->SetSize(size_);
	hpFrame_->Update();
}