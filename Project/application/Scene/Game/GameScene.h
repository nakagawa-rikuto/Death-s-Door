#pragma once
/// ===Include=== ///
#include "Engine/Scene/IScene.h"
// Entity
#include "application/Game/Entity/Player/Player.h"
#include "application/Game/Entity/Enemy/BossEnemy/BossEnemy.h"
// Object
#include "application/Game/Object/GameStage.h"
// State
#include "State/Base/GameSceneFadeState.h"

///=====================================================/// 
/// ゲームシーン
///=====================================================///
class GameScene : public MiiEngine::IScene {
public:/// ===メンバ関数=== ///

	GameScene();
	~GameScene();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	/// <summary>
	/// ゲームシーンのフェード状態を変更します。
	/// </summary>
	/// <param name="newState">新しいフェード状態へのユニークポインタ。</param>
	void ChangState(std::unique_ptr<GameSceneFadeState> newState);

public: /// ===Getter=== ///
	// Playerの取得
	Player* GetPlayer() const { return player_.get(); }
	// Enemyの取得
	BossEnemy* GetEnemy() const { return enemy_.get(); }
	// Camera 取得
	MiiEngine::FollowCamera* GetCamera() const { return camera_.get(); }

private:/// ===メンバ変数=== ///

	/// ===Classの宣言=== /// 
	// Camera
	std::unique_ptr<MiiEngine::FollowCamera> camera_;
	// Player
	std::unique_ptr<Player> player_;
	// Enemy
	std::unique_ptr<BossEnemy> enemy_;
	// Stage
	std::unique_ptr<GameStage> stage_;
	// State
	std::unique_ptr<GameSceneFadeState> currentState_;

	/// ===変数の宣言=== ///
	// カメラ情報
	struct CameraInfo {
		Quaternion rotate = { 0.372f, 0.0f, 0.0f, 1.055f };
	};
	CameraInfo cameraInfo_;

private: /// ===メンバ関数=== ///

	/// <summary>
	/// カメラの初期設定
	/// </summary>
	void SetUpCamera();

	/// <summary>
	/// パーティクルの追加
	/// </summary>
	void LoadParticle();
};