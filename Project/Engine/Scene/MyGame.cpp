#include "MyGame.h"
// シーンファクトリー
#include "Engine/Scene/SceneFactory.h"
#include "Engine/System/Service/Loader.h"
#include "Engine/Core/Logger.h"
// c++
#include <iostream>
#include <thread>
#include <chrono>

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
	// スレッドの生成
	std::thread loadingThread([this] {
		LoadAudio();
		LoadTexture();
		LoadModel();
		LoadAnimation();
	});

	// 終了を待機
	loadingThread.join();

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
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene(SceneType::Title);
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
	// 基底クラスの更新処理
	Framework::Update();
	// シーンの更新
	sceneManager_->Update();
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
/// 読み込み関数
///-------------------------------------------///
// 音
void MyGame::LoadAudio() {
#pragma region Wave
	Loader::LoadWave("fanfare", "fanfare.wav");
#pragma endregion

#pragma region MP3
	Loader::LoadMP3("clear", "clear.mp3");
#pragma endregion
}
// テクスチャ
void MyGame::LoadTexture() {
	// DebugSceneで使用。
	Loader::LoadTexture("uvChecker", "uvChecker.png");
	Loader::LoadTexture("monsterBall", "monsterBall.png");
	// Particleで使用。
	Loader::LoadTexture("circle", "circle.png");
	Loader::LoadTexture("circle2", "circle2.png");
	Loader::LoadTexture("gradationLine", "gradationLine.png");
	Loader::LoadTexture("noise0", "OffScreen/noise0.png");
	Loader::LoadTexture("noise1", "OffScreen/noise1.png");
	Loader::LoadTexture("skyBox", "rostock_laage_airport_4k.dds");

	// Game(Title)
	Loader::LoadTexture("Title", "TitleUI/Title.png");
	Loader::LoadTexture("Start", "TitleUI/Start.png");
	Loader::LoadTexture("Option", "TitleUI/Option.png");
	Loader::LoadTexture("Exit", "TitleUI/Exit.png");
	Loader::LoadTexture("OverLay", "TitleUI/OverLay.png");
}
// モデル
void MyGame::LoadModel() {
	// DebugSceneで使用。
	Loader::LoadModel("MonsterBall", "MonsterBall.obj");
	Loader::LoadModel("terrain", "terrain.obj");
	Loader::LoadModel("plane", "plane.obj");
	// Particleで使用。
	Loader::LoadModel("Particle", "Particle.obj");
	Loader::LoadModel("ParticlePlane", "ParticlePlane.gltf");
	// Game
	Loader::LoadModel("player", "player.gltf");
	Loader::LoadModel("GameGround", "Ground.obj");
}
// アニメーション
void MyGame::LoadAnimation() {
	// DebugSceneで使用。
	Loader::LoadAnimation("simpleSkin", "simpleSkin.gltf");
	Loader::LoadAnimation("human", "sneakWalk.gltf");
}