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
	std::shared_ptr<GameCamera> camera_;
	// Line
	std::unique_ptr<Line> line_;

	/// ===変数の宣言=== ///
	struct CameraInfo {
		Quaternion rotate = { 0.372f, 0.0f, 0.0f, 1.055f };
	};
	CameraInfo cameraInfo_;

private: /// ===メンバ関数=== ///
	/// <summary>
	/// 配置処理
	/// </summary>
	/// <param name="json_name">使用するJsonファイル名</param>
	void SpawnObjects(const std::string& json_name);
};

