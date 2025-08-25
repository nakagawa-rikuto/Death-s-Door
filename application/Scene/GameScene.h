#pragma once
/// ===Includ=== ///
#include "Engine/Scene/IScene.h"

// Game
#include "application/Game/Entity/Player/Player.h"
#include "application/Game/Object/Ground/Ground.h"
#include "application/Game/Entity/Enemy/Manager/EnemyManager.h"

///=====================================================/// 
/// ゲームシーン
///=====================================================///
class GameScene : public IScene {
public:/// ===メンバ関数=== ///

	GameScene() = default;
	~GameScene();

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
	
	/// ===Classの宣言=== /// 
	// Player
	std::unique_ptr<Player> player_;
	// Enemy
	std::unique_ptr<EnemyManager> enemyManager_;
	// Ground
	std::unique_ptr<Ground> ground_;
	// Camera
	std::shared_ptr<Camera> camera_;
	// Line
	std::unique_ptr<Line> line_;

	/// ===変数の宣言=== ///
	struct CameraInfo {
		Quaternion rotate = { 0.372f, 0.0f, 0.0f, 1.055f };
	};
	CameraInfo cameraInfo_;
};

