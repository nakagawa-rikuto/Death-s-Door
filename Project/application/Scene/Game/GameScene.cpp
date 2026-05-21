#include "GameScene.h"
// SceneManager
#include "Engine/System/Managers/SceneManager.h"
// Service
#include "Service/Camera.h"
#include "Service/Particle.h"
#include "Service/Collision.h"
#include "Service/GraphicsResourceGetter.h"
#include "Service/Audio.h"
// State
#include "State/GameSceneInitializeState.h"
// Math
#include "Math/SMath.h"
#ifdef USE_IMGUI
// Service
#include "Service/Input.h"
#endif // USE_IMGUI

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
GameScene::GameScene() {
	/// ===パーティクルの読み込み=== ///
	LoadParticle();
}

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
GameScene::~GameScene() {
	// BGMの停止
	//AudioService::StopSound("title");
	// Colliderのリセット
	Service::Collision::Reset();
	// Camera
	Service::Camera::Remove("Game");
	camera_.reset();
	// State
	if (currentState_) {
		currentState_->Finalize();
		currentState_.reset();
	}
	// Particle
	Service::Particle::RemoveAllParticles();
	// Enemy
	enemy_.reset();
	// Player
	player_.reset();
	// Ground
	stage_.reset();
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void GameScene::Initialize() {
	/// ===ISceneの初期化=== ///
	IScene::Initialize();

	/// ===Camera=== ///
	camera_ = std::make_unique<MiiEngine::FollowCamera>();
	camera_->Initialize();
	SetUpCamera();
	// Managerに追加,アクティブに
	Service::Camera::AddCamera("Game", camera_.get());
	Service::Camera::SetActiveCamera("Game");

	/// ===GameStage=== ///
	stage_ = std::make_unique<GameStage>();
	stage_->Initialize("Level/BossStage.json");

	/// ===Playerの生成=== ///
	player_ = std::make_unique<Player>();
	Vector3 translation = { 0.0f, 5.0f, -30.0f };
	player_->InitGame(translation, camera_.get());
	player_->SetGroundOcean(stage_->GetGroundOcean());

	/// ===EnemyManagerの生成=== ///
	enemy_ = std::make_unique<BossEnemy>();
	enemy_->InitGameScene({ 0.0f, 8.0f, 30.0f });
	enemy_->SetPlayer(player_.get());
	enemy_->SetGroundOcean(stage_->GetGroundOcean());

	/// ===State=== ///
	// 初期状態をInitializeStateに設定
	ChangState(std::make_unique<GameSceneInitializeState>());

	// ColliderServiceのライト設定
	Service::Collision::SetLight(MiiEngine::LightType::HalfLambert);
	lightInfo_ = {
		.shininess = 32.0f, // 光沢度を少し高めにしてハイライトを綺麗に
		.directional = {
			.color = { 1.0f, 0.98f, 0.9f, 1.0f }, // わずかに暖かみのある色
			.direction = { -0.5f, -0.8f, 0.5f },  // 斜め少し前・上空からキャラクターを照らす向き
			.intensity = 0.8f,
		},
		.point {
			.color = { 1.0f, 0.6f, 0.2f, 1.0f },  // 温かみのあるオレンジ系の環境アクセント
			.position = { 0.0f, 30.0f, 0.0f },     // プレイヤーの少し上
			.intensity = 30.0f,                    // ライトの強さ
			.radius = 1000.0f,                      // プレイヤー周辺を十分覆う範囲
			.decay = 1.5f,                        // 距離に応じた自然な減衰
		},
		.spot = {
			.color = { 0.8f, 0.9f, 1.0f, 1.0f },  // スポットライトは少しクールな青白系
			.position = { 0.0f, 8.0f, 0.0f },     // プレイヤーの真上
			.intensity = 1.5f,                    // 少し強めに強調
			.direction = { 0.0f, -1.0f, 0.0f },   // 真下を向く
			.distance = 15.0f,                    // 地面に届く十分な距離
			.decay = 2.0f,                        // スポット外周にかけての減衰率
			.cosAngle = 0.707f,                   // 約45度の照射角 (cos(45度) ≒ 0.707)
		},
	};
	Service::Collision::SetLightData(lightInfo_);

	// BGMの再生
	//AudioService::StartSound("title", true);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void GameScene::Update() {
	/// ===デバック用ImGui=== ///
#ifdef USE_IMGUI
	ImGui::Begin("GameScene");
	ImGui::End();

	// Light
	ImGui::Begin("ライト情報");
	/// ライトの種類の選択
	static int lightType = static_cast<int>(MiiEngine::LightType::HalfLambert);
	ImGui::Text("Light Type");
	if (ImGui::RadioButton("Lambert", &lightType, static_cast<int>(MiiEngine::LightType::Lambert))) {
		Service::Collision::SetLight(MiiEngine::LightType::Lambert);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("HalfLambert", &lightType, static_cast<int>(MiiEngine::LightType::HalfLambert))) {
		Service::Collision::SetLight(MiiEngine::LightType::HalfLambert);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("PointLight", &lightType, static_cast<int>(MiiEngine::LightType::PointLight))) {
		Service::Collision::SetLight(MiiEngine::LightType::PointLight);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("SpotLight", &lightType, static_cast<int>(MiiEngine::LightType::SpotLight))) {
		Service::Collision::SetLight(MiiEngine::LightType::SpotLight);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("None", &lightType, static_cast<int>(MiiEngine::LightType::None))) {
		Service::Collision::SetLight(MiiEngine::LightType::None);
	}

	ImGui::DragFloat("光沢度", &lightInfo_.shininess, 0.01f);

	if (ImGui::TreeNode("Directionl")) {
		ImGui::ColorEdit3("ライトの色", &lightInfo_.directional.color.x);
		ImGui::DragFloat3("ライトの方向", &lightInfo_.directional.direction.x, 0.01f);
		ImGui::DragFloat("ライトの強さ", &lightInfo_.directional.intensity, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Point")) {
		ImGui::ColorEdit3("ライトの色", &lightInfo_.point.color.x);
		ImGui::DragFloat3("ライトの位置", &lightInfo_.point.position.x, 0.01f);
		ImGui::DragFloat("ライトの強さ", &lightInfo_.point.intensity, 0.01f);
		ImGui::DragFloat("ライトの半径", &lightInfo_.point.radius, 0.01f);
		ImGui::DragFloat("ライトの減衰率", &lightInfo_.point.decay, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Spot")) {
		ImGui::ColorEdit3("ライトの色", &lightInfo_.spot.color.x);
		ImGui::DragFloat3("ライトの位置", &lightInfo_.spot.position.x, 0.01f);
		ImGui::DragFloat3("ライトの方向", &lightInfo_.spot.direction.x, 0.01f);
		ImGui::DragFloat("ライトの強さ", &lightInfo_.spot.intensity, 0.01f);
		ImGui::DragFloat("ライトの距離", &lightInfo_.spot.distance, 0.01f);
		ImGui::DragFloat("ライトの減衰率", &lightInfo_.spot.decay, 0.01f);
		ImGui::DragFloat("ライトのコサイン角度", &lightInfo_.spot.cosAngle, 0.01f);
		ImGui::TreePop();
	}
	Service::Collision::SetLightData(lightInfo_);
	ImGui::End();

	// Camera
	if (Service::Camera::GetActiveCamera() == camera_.get()) {
		camera_->ImGuiUpdate();
		camera_->DebugUpdate();
	} else {
		defaultCamera_->ImGuiUpdate();
		defaultCamera_->DebugUpdate();
	}

	// デバッグカメラの切り替え
	if (Service::Input::TriggerKey(DIK_TAB)) {
		if (Service::Camera::GetActiveCamera() == camera_.get()) {
			Service::Camera::SetActiveCamera("Default");
		} else {
			Service::Camera::SetActiveCamera("Game");
		}
	}

	// Player
	player_->Information();
	// Enemy
	enemy_->Information();

#endif // USE_IMGUI

	/// ===EntityのPreUpdate=== ///
	player_->PreUpdate(); // Playerの更新前処理
	enemy_->PreUpdate();  // Enemyの更新前処理

	/// ===Groundの更新=== ///
	stage_->Update();

	/// ===Stateの管理=== ///
	if (currentState_) {
		// 各Stateの更新
		currentState_->Update();
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void GameScene::Draw() {
	/// ===Enemy=== ///
	enemy_->Draw();

	/// ===Player=== ///
	player_->Draw();

	/// ===GameStage=== ///
	stage_->Draw();

	/// ===Stateの管理=== ///
	if (currentState_) {
		// 各Stateの更新
		currentState_->Draw();
	}
}

///-------------------------------------------/// 
/// Stateの変更処理
///-------------------------------------------///
void GameScene::ChangState(std::unique_ptr<GameSceneFadeState> newState) {
	if (currentState_) {
		// 古い状態を解放  
		currentState_->Finalize();
		currentState_.reset();
	}

	// 新しい状態をセット  
	currentState_ = std::move(newState);
	// SceneManagerの設定
	currentState_->SetSceneManager(sceneManager_);
	// 新しい状態の初期化  
	currentState_->Enter(this);
}

///-------------------------------------------/// 
/// カメラの初期設定
///-------------------------------------------///
void GameScene::SetUpCamera() {
	/// ===カメラの初期設定=== ///
	// カメラの座標設定
	camera_->SetTranslate({ 0.0f, 5.0f, -20.0f });
	// カメラの回転設定
	camera_->SetRotate({ 0.0f, 0.0f, 0.0f, 1.0f });
	// カメラの追従設定
	camera_->SetFollowCamera(MiiEngine::FollowCameraType::TopDown);
	// 追従オフセット設定
	camera_->SetOffset({ 0.0f, 100.0f, -90.0f });
	// 追従速度設定
	camera_->SetFollowSpeed(0.1f);
}

///-------------------------------------------/// 
/// パーティクルの読み込み
///-------------------------------------------///
void GameScene::LoadParticle() {
	// Player
	Service::Particle::LoadParticleDefinition("PlayerMove.json");
	Service::Particle::LoadParticleDefinition("PlayerAttack.json");
	Service::Particle::LoadParticleDefinition("PlayerHitEffect1.json");

	// Boss
	Service::Particle::LoadParticleDefinition("EnemyAttack.json");
	Service::Particle::LoadParticleDefinition("EnemyHitEffect.json");
	Service::Particle::LoadParticleDefinition("EnemyTeleportParticle.json");
	Service::Particle::LoadParticleDefinition("EnemyDeathParticle.json");
	Service::Particle::LoadParticleDefinition("Charge.json");
}
