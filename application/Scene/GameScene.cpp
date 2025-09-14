#include "GameScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
// Service
#include "Engine/System/Service/CameraService.h"
#include "Engine/System/Service/ParticleService.h"
#include "Engine/System/Service/ColliderService.h"
#include "Engine/System/Service/InputService.h"
#include "Engine/System/Service/GraphicsResourceGetter.h"
// Particle
#include "Engine/Graphics/Particle/Derivative/ConfettiParticle.h"
#include "Engine/Graphics/Particle/Derivative/ExplosionParticle.h"
#include "Engine/Graphics/Particle/Derivative/WindParticle.h"
#include "Engine/Graphics/Particle/Derivative/HitEffectParticle.h"
#include "Engine/Graphics/Particle/Derivative/RingParticle.h"
#include "Engine/Graphics/Particle/Derivative/CylinderParticle.h"
// Math
#include "Math/SMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
GameScene::~GameScene() {
	// ISceneのデストラクタ
	IScene::~IScene();
	// Colliderのリセット
	ColliderService::Reset();
	// Camera
	camera_.reset();
	// Player
	player_.reset();
	// Ground
	ground_.reset();
	// Enemy
	enemyManager_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void GameScene::Initialize() {
	// ISceneの初期化(デフォルトカメラとカメラマネージャ)
	IScene::Initialize();

	/// ===Camera=== ///
	camera_ = std::make_shared<Camera>();
	camera_->Initialize();
	camera_->SetRotate(cameraInfo_.rotate);
	camera_->SetFollowCamera(FollowCameraType::TopDown);
	camera_->SetOffset({ 0.0f, 70.0f, -60.0f });
	camera_->SetFollowSpeed(0.1f);
	// Managerに追加,アクティブに
	CameraService::Add("Game", camera_);
	CameraService::SetActiveCamera("Game");

	/// ===Player=== ///
	player_ = std::make_unique<Player>();
	player_->Initialize();

	/// ===Enemy=== ///
	enemyManager_ = std::make_unique<EnemyManager>();
	//enemyManager_->Spawn(EnemyType::CloseRange, { -10.0f, 1.0f, 10.0f });
	enemyManager_->Spawn(EnemyType::CloseRange, { 10.0f, 1.0f, 10.0f });
	enemyManager_->Spawn(EnemyType::CloseRange, { 10.0f, 1.0f, -10.0f });
	enemyManager_->Spawn(EnemyType::CloseRange, {- 10.0f, 1.0f, -10.0f });
	//enemyManager_->Spawn(EnemyType::LongRange, { 10.0f, 1.0f, 10.0f });
	// EnemyのSpaw後に呼ぶ
	enemyManager_->SetPlayer(player_.get()); // Playerを設定
	/// ===Ground=== ///
	ground_ = std::make_unique<Ground>();
	ground_->Initialize();

	/// ===Line=== ///
	line_ = std::make_unique<Line>();

	// Particleの追加
	ParticleService::AddParticle("Confetti", std::make_unique<ConfettiParticle>());
	ParticleService::AddParticle("Explosion", std::make_unique<ExplosionParticle>());
	ParticleService::AddParticle("Wind", std::make_unique<WindParticle>());
	ParticleService::AddParticle("Ring", std::make_unique<RingParticle>());
	ParticleService::AddParticle("HitEffect", std::make_unique<HitEffectParticle>());
	ParticleService::AddParticle("Cylinder", std::make_unique<CylinderParticle>());
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void GameScene::Update() {
	/// ===デバック用ImGui=== ///
#ifdef USE_IMGUI
	ImGui::Begin("GameScene");
	ImGui::End();

	// Camera
	camera_->UpdateImGui();

	// Player
	player_->UpdateImGui();

	// Enemy
	enemyManager_->UpdateImGui();

#endif // USE_IMGUI

	/// ===DebugCamera=== ///
	camera_->DebugUpdate();

	/// ===Playerの更新=== ///
	player_->Update();

	/// ===Enemy=== ///
	enemyManager_->Update();

	/// ===Groundの更新=== ///
	ground_->Update();

	/// ===ColliderManager=== ///

	/// ===ISceneの更新=== ///
	IScene::Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void GameScene::Draw() {
#pragma region 背景スプライト描画

#pragma endregion

#pragma region モデル描画

	line_->DrawGrid({ 0.0f,-2.0f, 0.0f }, { 1000.0f, 1.0f, 1000.0f }, 100, { 1.0f, 1.0f, 1.0f, 1.0f });

	// Ground
	//ground_->Draw();
	// Enemy
	enemyManager_->Draw();
	// Player
	player_->Draw();

	/// ===ISceneの描画=== ///
	IScene::Draw();
#pragma endregion

#pragma region 前景スプライト描画
#pragma endregion
}

///-------------------------------------------/// 
/// 配置関数
///-------------------------------------------///
void GameScene::SpawnObjects(const std::string& json_name) {
	LevelData* levelData = GraphicsResourceGetter::GetLevelData(json_name);

	// オブジェクト分回す
	for (const auto& obj : levelData->objects) {

		/// ===クラス名で分岐=== ///
		switch (obj.classType) {
		case LevelData::ClassType::Player1:
			// 初期化と座標設定
			player_->Initialize();
			player_->SetTranslate(obj.translation);
			player_->SetRotate(Math::QuaternionFromVector(obj.rotation));
			break;
		case LevelData::ClassType::Enemy1:
			// Enemyの座標設定
			enemyManager_->Spawn(EnemyType::LongRange, obj.translation);
			break;
		case LevelData::ClassType::Enemy2:
			// Enemyの座標設定
			enemyManager_->Spawn(EnemyType::CloseRange, obj.translation);
			break;
		case LevelData::ClassType::Object1:
			//NOTE:Objectクラス（名所はGPTに聞く）を作成。当たり判定を付けれるようにする。
			break;
		case LevelData::ClassType::Ground1:
			//NOTE:今後地面を複数個作成する可能性あり。当たり判定を付けれるようにする。
			ground_->Initialize();
			break;
		default:
			break;
		}
	}
}
