#pragma once
/// ===Include=== ///
#include "application/Drawing/2d/Object2d.h"
#include <application/Game/UI/HP/HpUI.h>

/// ===前方宣言=== ///
class Player;

///-------------------------------------------/// 
/// プレイヤーUI
///-------------------------------------------///
class PlayerUI {
public:

	PlayerUI() = default;
	~PlayerUI();

	/// <summary>
	/// プレイヤーとウィンドウサイズを使用して初期化します。
	/// </summary>
	/// <param name="player">初期化するプレイヤーへのポインター。</param>
	/// <param name="windowSize">ウィンドウのサイズ。</param>
	void Initialize(Player* player, const Vector2& windowSize);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

private:
	// Player
	Player* player_;

	/// ===Object2d=== ///
	// アクション
	std::unique_ptr<Object2d> moveUI_;      // 移動UI
	std::unique_ptr<Object2d> attackUI_;    // 攻撃UI
	std::unique_ptr<Object2d> dodgeUI_;		// 回避UI
	std::unique_ptr<Object2d> attackDirectionUI_; // 攻撃方向UI
	// コントローラー
	std::unique_ptr<Object2d> xButton_;		// X
	std::unique_ptr<Object2d> aButton_;		// A
	std::unique_ptr<Object2d> leftStick_;	// 左スティック
	// HPUI
	std::unique_ptr<HpUI> hpUI_;			// HPUI

	/// ===位置=== ///
	Vector2 moveUIPos_ = { 100.0f, 500.0f };      // 移動UI位置

	/// ===攻撃方向UI=== ///
	float attackDirectionMinDistance_ = 0.0f; // 攻撃方向UIの最小距離

	/// ===Color=== ///
	Vector4 baseColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };     // 基本カラー
	Vector4 activeColor_ = { 1.0f, 0.0f, 0.0f, 1.0f };   // アクティブカラー
	Vector4 cooldownColor_ = { 1.0f, 1.0f, 0.0f, 1.0f }; // クールダウンカラー

	/// ===UI情報=== ///
	struct UIInfo {
		Vector2 size;
		Vector2 activeSize;
		Vector2 anchorPoint;
		Vector4 color;
	};
	UIInfo uiInfo_{};

	/// ===フラグ=== ///
	struct ColorChange {
		bool attackUI = false;		  // 攻撃UIアクティブフラグ
		bool dodgeActiveUI = false;   // 回避UIアクティブフラグ
		bool dodgeCooldownUI = false; // 回避UIクールダウンフラグ
	};
	ColorChange colorChange_;

	/// ===参照スケール倍率=== ///
	Vector2 scale_ = { 1.0f, 1.0f }; // スケール

private:

	/// <summary>
	/// コントローラーUIをセットアップします。
	/// </summary>
	/// <param name="windowSize">ウィンドウのサイズ。</param>
	void SetUpControllerUI(const Vector2& windowSize);

	/// <summary>
	/// 攻撃方向UIをセットアップします。
	/// </summary>
	/// <param name="windowSize">ウィンドウのサイズ。</param>
	void SetUpAttackDirectionUI(const Vector2& windowSize);

	/// <summary>
	/// HPUIをセットアップします。
	/// </summary>
	/// <param name="windowSize">ウィンドウのサイズ。</param>
	void SetUpHPUI(const Vector2& windowSize);

	/// <summary>
	/// スプライトの更新処理
	/// </summary>
	void SpriteUpdate();

	/// <summary>
	/// 色の更新処理
	/// </summary>
	void ColorUpdate();

	/// <summary>
	/// 攻撃方向UIの更新処理
	/// マウスカーソル位置に追従させ、Playerから見たマウス方向を向くように回転させる
	/// </summary>
	void AttackDirectionUpdate();

	/// <summary>
	/// デバイス変更時の処理
	/// </summary>
	void ChangeDevice();
};

