#pragma once
/// ===Include=== ///
// IScene
#include "Engine/Scene/IScene.h"
#include "application/Drawing/2d/Sprite.h"
#include "application/Drawing/3d/Object3d.h"

// Transition
#include "Engine/Scene/Transition/SceneTransition.h"

#include <memory>
#include <array>

// メニュー項目の列挙型
enum class MenuSelection {
	Start,    // 開始
	Option,   // オプション
	Exit      // 終了
};

///=====================================================/// 
/// タイトルシーン
///=====================================================///
class TitleScene : public IScene {
public:/// ===メンバ関数=== ///

	TitleScene() = default;
	~TitleScene();

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;

private:/// ===メンバ変数=== ///
	/// <summary>
	/// シーン用
	/// </summary>

	/// ===SceneTransition=== ///
	std::unique_ptr<SceneTransition> transiton_;

	/// ===スプライト=== ///
	std::unique_ptr<Sprite> bgSprite_;         // 背景
	std::unique_ptr<Sprite> bgKiriSprite_;         // 背景
	std::unique_ptr<Sprite> titleSprite_;      // タイトル
	std::unique_ptr<Sprite> startSprite_;      // 開始
	std::unique_ptr<Sprite> optionSprite_;     // オプション
	std::unique_ptr<Sprite> exitSprite_;       // 終了
	std::unique_ptr<Sprite> selectOverlay_;    // 選択中のオーバーレイ
	std::unique_ptr<Sprite> dimSprite_;        // 薄暗いオーバーレイ
	std::unique_ptr<Sprite> optionMenuSprite_; // オプションメニュー

	/// ===モデル=== ///
	static constexpr int kModelCount = 3; // モデルの数
	std::array<std::unique_ptr<Object3d>, kModelCount> models_; // 3Dモデル配列
	std::array<std::string, kModelCount> modelNames_ = { "player", "player", "player" }; // モデル名

	int currentModelIndex_ = 0;  // 現在表示中のモデルのインデックス
	bool isTransitioning_ = false; // トランジション中かどうか
	float transitionTimer_ = 0.0f; // トランジションタイマー
	float transitionDuration_ = 0.5f; // トランジションの時間（秒）
	int nextModelIndex_ = 0; // 次に表示するモデルのインデックス
	bool transitionDirection_ = true; // true: 右へ, false: 左へ

	/// ===メニュー選択=== ///
	MenuSelection currentSelection_ = MenuSelection::Start; // 現在の選択
	bool isOptionOpen_ = false;                              // オプション画面が開いているか

	/// ===入力制御=== ///
	bool canInput_ = true; // 入力可能フラグ（連続入力防止）

private:/// ===プライベート関数=== ///

	// メニュー選択の更新
	void UpdateMenuSelection();
	// メニュー決定処理
	void ConfirmSelection();
	// メニュー項目の色更新
	void UpdateSelectOverlayPosition();
	// オプション画面の更新
	void UpdateOptionMenu();

	// モデル選択の更新
	void UpdateModelSelection();
	// モデルトランジションの更新
	void UpdateModelTransition();
	// イージング関数
	float EaseInOutCubic(float t);

};