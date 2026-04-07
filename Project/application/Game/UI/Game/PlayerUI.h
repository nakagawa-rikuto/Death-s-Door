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
	std::unique_ptr<Object2d> cameraUI_;    // カメラUI
	std::unique_ptr<Object2d> attackUI_;    // 攻撃UI
	std::unique_ptr<Object2d> avoidanceUI_; // 回避UI
	// コントローラー
	std::unique_ptr<Object2d> xButton_;		// X
	std::unique_ptr<Object2d> aButton_;		// A
	std::unique_ptr<Object2d> leftStick_;	// 左スティック
	std::unique_ptr<Object2d> rightStick_;  // 右スティック
	// HPUI
	std::unique_ptr<HpUI> hpUI_;			// HPUI

	/// ===位置=== ///
	Vector2 moveUIPos_ = { 100.0f, 500.0f };      // 移動UI位置
	Vector2 cameraUIPos_ = { 300.0f, 500.0f };    // カメラUI位置
	Vector2 hpUIPos_ = { 0.0f, 0.0f };			  // HPUI位置
	Vector2 hpUIOffset_ = { 0.0f, 0.0f };			  // HPUIのオフセット

	/// ===Color=== ///
	Vector4 activeColor_ = { 1.0f, 0.0f, 0.0f, 1.0f };   // アクティブカラー

	/// ===フラグ=== ///
	struct ColorChange {
		bool attackUI = false;    // 攻撃UIアクティブフラグ
		bool avoidanceUI = false; // 回避UIアクティブフラグ
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
	/// スプライトの更新処理
	/// </summary>
	void SpriteUpdate();

	/// <summary>
	/// 色の更新処理
	/// </summary>
	void ColorUpdate();
};

