#pragma once
/// ===Include=== ///
#include "application/Drawing/2d/Object2d.h"

///=====================================================/// 
/// HPUI
///=====================================================///
class HpUI {
public:

	HpUI() = default;
	~HpUI();

	/// <summary>
	/// 位置とサイズで初期化します。
	/// </summary>
	/// <param name="position">初期位置。</param>
	/// <param name="size">サイズ。</param>
	/// <param name="maxHP">最大ヒットポイント。</param>
	void Initialize(const Vector2& position, const Vector2& size, float maxHp);

	/// <summary>
	/// 現在のHPと最大HPに基づいて更新します。
	/// </summary>
	/// <param name="deltaTIme">デルタタイム（秒）。</param>
	/// <param name="currentHP">現在のヒットポイント。</param>
	void Update(float deltaTime, float currentHP);

	/// <summary>
	/// 位置を設定します。
	/// </summary>
	/// <param name="position">設定する位置</param>
	void SetPosition(const Vector2& position) { position_ = position; }

private:
	/// ===Sprite=== ///
	std::unique_ptr<Object2d> hpBackground_;	// HPバーの背景（黒）
	std::unique_ptr<Object2d> hpDamage_;		// ダメージ遅延表示（赤）
	std::unique_ptr<Object2d> hpBar_;			// 現在HP表示（緑）
	std::unique_ptr<Object2d> hpFrame_;			// HPバーの枠

	/// ===位置・サイズ=== ///
	Vector2 position_ = {};						// HPバーの基準位置
	Vector2 size_ = {};							// HPバーの全体サイズ

	/// ===HP管理=== ///
	float displayHP_ = 1.0f;					// 現在表示するHP割合（緑バー）
	float damageHP_ = 1.0f;						// ダメージ遅延表示のHP割合（赤バー）
	float previousHP_ = 1.0f;					// 前フレームのHP割合
	float maxHP_ = 100.0f;						// 最大HP

	/// ===ダメージ遅延アニメーション=== ///
	float damageTimer_ = 0.0f;					// ダメージ後の経過時間
	float damageDelay_ = 1.2f;					// 赤バーが消え始めるまでの待機時間（秒）
	float damageDecreaseSpeed_ = 3.0f;			// 赤バーの減少速度（割合 / 秒）
	bool isDamageVisible_ = false;				// 赤バー表示中かどうか

	/// ===スケール=== ///
	Vector2 scale_ = { 1.0f, 1.0f };			// 参照スケール倍率

private:
	/// <summary>
	/// スプライトの位置・サイズを更新します。
	/// </summary>
	void UpdateSprites();
};

