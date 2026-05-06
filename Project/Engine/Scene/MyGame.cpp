#include "MyGame.h"
// シーンファクトリー
#include "Engine/Scene/SceneFactory.h"
// Service
#include "Service/Loader.h"
#include "Service/Particle.h"
// Logger
#include "Engine/Core/Logger.h"
// c++
#include <iostream>
#include <thread>
#include <chrono>

using namespace Service;
namespace MiiEngine {
	///-------------------------------------------/// 
	/// コンストラクタ、デストラクタ
	///-------------------------------------------///
	MyGame::MyGame() = default;
	MyGame::~MyGame() {
		sceneFactory_.reset();
		sceneManager_.reset();
	}

	///-------------------------------------------/// 
	/// 初期化
	///-------------------------------------------///
	void MyGame::Initialize(const wchar_t* title) {
		// 基底クラスの初期化
		Framework::Initialize(title);

		/// ===時間計測=== ///
		auto start = std::chrono::high_resolution_clock::now();

		/// ===読み込み処理=== ///
		// 各読み込み処理用のスレッドを生成
		LoadAudio();	// Soundの読み込み
		std::vector<std::thread> loadingThreads;
		loadingThreads.emplace_back([this] { LoadJson(); });	  // Jsonデータの読み込み
		loadingThreads.emplace_back([this] {
			LoadTexture();   // Textureの読み込み
			LoadModel();     // Modelの読み込み
			LoadAnimation(); // Animationの読み込み
			});
		// すべてのスレッドの終了を待機
		for (auto& thread : loadingThreads) {
			thread.join();
		}

		// 処理時間を計測（end）
		auto end = std::chrono::high_resolution_clock::now();
		// 経過時間をミリ秒単位で出力
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		Log("time: " + std::to_string(duration.count()) + " ms\n");

		/// ===シーンの作成=== ///
		// シーンファクトリーの生成
		sceneFactory_ = std::make_unique<SceneFactory>();
		// シーンマネージャの初期化
		sceneManager_ = std::make_unique<SceneManager>();
		sceneManager_->Initialize(sceneFactory_.get(), spriteManager_.get());
		sceneManager_->ChangeScene(SceneType::Game);   //　スタートシーンの設定
	}

	///-------------------------------------------/// 
	/// 終了処理
	///-------------------------------------------///
	void MyGame::Finalize() {
		// シーンの解放処理
		sceneManager_.reset();
		sceneFactory_.reset();
		// 基底クラスの終了処理
		Framework::Finalize();
	}

	///-------------------------------------------/// 
	/// 更新
	///-------------------------------------------///
	void MyGame::Update() {
		// シーンの更新
		sceneManager_->Update();

		// Frameworkの更新
		Framework::Update();
	}

	///-------------------------------------------/// 
	/// 描画
	///-------------------------------------------///
	void MyGame::Draw() {
		// 描画前処理
		Framework::PreDraw();
		// シーンの描画
		sceneManager_->Draw();
		// 描画後処理
		Framework::PostDraw();
	}


	///-------------------------------------------/// 
	/// Soundの読み込み関数
	///-------------------------------------------///
	void MyGame::LoadAudio() {
#pragma region Wave
		Loader::LoadWave("title", "title.wav");
#pragma endregion
#pragma region MP3
		Loader::LoadMP3("clear", "clear.mp3");
#pragma endregion
	}

	///-------------------------------------------/// 
	///	テクスチャの読み込み処理
	///-------------------------------------------///
	void MyGame::LoadTexture() {
		/// ===Engine=== ///
		Loader::LoadTexture("uvChecker", "uvChecker.png");
		Loader::LoadTexture("monsterBall", "monsterBall.png");
		Loader::LoadTexture("White", "WhiteTexture.png");
		// Particle
		Loader::LoadTexture("circle", "Particle/circle.png");
		Loader::LoadTexture("circle2", "Particle/circle2.png");
		Loader::LoadTexture("gradationLine", "Particle/gradationLine.png");
		Loader::LoadTexture("Spark", "Particle/Spark.png");
		Loader::LoadTexture("FlameEye", "Particle/FlameEye.png");
		Loader::LoadTexture("Fire", "Particle/Fire.png");
		// OffScreen
		Loader::LoadTexture("noise0", "OffScreen/noise0.png");
		Loader::LoadTexture("noise1", "OffScreen/noise1.png");
		// Ocean
		Loader::LoadTexture("oceanFFT", "Ocean/foam_noise.png");
		// DDS
		Loader::LoadTexture("skyBox", "DDS/rostock_laage_airport_4k.dds");

		/// ===Game=== ///
		// SceneTransition
		Loader::LoadTexture("ShatterGlass", "Animation/SceneTransitionSpirte.png");
		// TitleUI
		Loader::LoadTexture("TitleBG", "TitleUI/SkyBG.png");
		Loader::LoadTexture("TitleBGKiri", "TitleUI/BGsecond.png");
		Loader::LoadTexture("Title", "TitleUI/Title.png");
		Loader::LoadTexture("Start", "TitleUI/Start.png");
		Loader::LoadTexture("Option", "TitleUI/Option.png");
		Loader::LoadTexture("Exit", "TitleUI/Exit.png");
		Loader::LoadTexture("OverLay", "TitleUI/OverLay.png");
		// GameUI
		Loader::LoadTexture("MoveUI", "GameUI/MoveUI.png");
		Loader::LoadTexture("CameraUI", "GameUI/CameraUI.png");
		Loader::LoadTexture("AttackUI", "GameUI/AttackUI.png");
		Loader::LoadTexture("AvoidanceUI", "GameUI/AvoidanceUI.png");
		Loader::LoadTexture("xButton", "GameUI/xbox_button_color_x.png");
		Loader::LoadTexture("aButton", "GameUI/xbox_button_color_a.png");
		Loader::LoadTexture("leftStick", "GameUI/xbox_stick_l.png");
		Loader::LoadTexture("rightStick", "GameUI/xbox_stick_r.png");
		Loader::LoadTexture("menuButton", "GameUI/xbox_button_menu.png");
		// OptionUI
		Loader::LoadTexture("OptionTitle", "OptionUI/OptionTitle.png");
		Loader::LoadTexture("OptionVolume", "OptionUI/Volume.png");
		// GameAnimation
		Loader::LoadTexture("GameOverAnimation", "Animation/GameOverAnimation.png");
	}

	///-------------------------------------------/// 
	/// モデルの読み込み処理
	///-------------------------------------------///
	void MyGame::LoadModel() {
		/// ===Particle=== ///
		Loader::LoadModel("plane", "Particle/Plane/ParticlePlane.gltf");
		Loader::LoadModel("sphere", "Particle/Sphere/ParticleSphere.obj");
		Loader::LoadModel("cube", "Particle/Cube/ParticleCube.obj");
		Loader::LoadModel("triangle", "Particle/Triangle/ParticleTriangle.obj");

		/// ===Entity=== ///
		Loader::LoadModel("Player", "Entity/Player/Player.gltf");							// プレイヤー
		Loader::LoadModel("PlayerHand", "Entity/Player/PlayerHand/PlayerHand.gltf");		// プレイヤー手
		Loader::LoadModel("PlayerWeapon", "Entity/Player/PlayerWeapon/PlayerWeapon.gltf");	// プレイヤー武器
		Loader::LoadModel("LongEnemy", "Entity/Enemy/LongEnemy/LongEnemy.gltf");			// 遠距離敵
		Loader::LoadModel("CloseEnemy", "Entity/Enemy/CloseEnemy/CloseEnemy.gltf");			// 近距離敵
		Loader::LoadModel("Boss", "Entity/Enemy/Boss/Boss.gltf");							// ボス
		Loader::LoadModel("BossWeapon", "Entity/Enemy/Boss/Weapon/BossWeapon.gltf");		// ボス武器

		/// ===Object=== ///
		Loader::LoadModel("Ground", "Object/Ground/Ground2.gltf");
		Loader::LoadModel("Ground2", "Object/Ground/Ground.gltf");
		Loader::LoadModel("Object", "Object/Object/Wall/BossStageWall.gltf");
	}

	///-------------------------------------------/// 
	/// アニメーションモデルの読み込み処理
	///-------------------------------------------///
	void MyGame::LoadAnimation() {
		/// ===Engine=== ///
		Loader::LoadAnimation("simpleSkin", "simpleSkin/simpleSkin.gltf");
		Loader::LoadAnimation("human", "human/sneakWalk.gltf");
	}

	///-------------------------------------------/// 
	/// Jsonデータの読み込み処理
	///-------------------------------------------///
	void MyGame::LoadJson() {
		/// ===Game=== ///
		Loader::LoadLevelJson("Level/BossStage.json");
	}
} 