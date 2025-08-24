#include "IScene.h"
// Service
#include "Engine/System/Service/ServiceLocator.h"
#include "Engine/System/Service/CameraService.h"
#include "Engine/System/Service/GraphicsResourceGetter.h"
#include "Engine/System/Service/Loader.h"
// Object3d
#include "Engine/Graphics/3d/Model/Model.h"
// Line
#include "Engine/Graphics/3d/Line/LineObject3D.h"
// Math
#include "Math/sMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
IScene::~IScene() {
	defaultCamera_.reset();
	models_.clear();
}

///-------------------------------------------/// 
/// Getter
///-------------------------------------------///
const float IScene::GetDeltaTime() const { return deltaTime_; }


///-------------------------------------------/// 
/// Setter
///-------------------------------------------///
void IScene::SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }
void IScene::SetDeltaTime(const float deltaTime) { deltaTime_ = deltaTime; }

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void IScene::Initialize() {

	/// ===デフォルトカメラの生成=== ///
	defaultCamera_ = std::make_shared<Camera>();
	defaultCamera_->Initialize();
	defaultCamera_->SetTranslate({ 0.0f, 0.0f, -10.0f });
	defaultCamera_->SetRotate({ 0.0f, 0.0f, 0.0f });

	// デフォルトカメラの設定
	CameraService::Add("Default", defaultCamera_);
	CameraService::SetActiveCamera("Default");

	// DeltaTimeの設定
	deltaTime_ = 1.0f / 60.0f;
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void IScene::Update() {
	// Line更新
	ServiceLocator::GetLineObject3D()->Update();
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void IScene::Draw() {

	// Lineの描画
	ServiceLocator::GetLineObject3D()->Draw();
}

///-------------------------------------------/// 
/// LevelDataからModelを生成して配置する
///-------------------------------------------///
void IScene::GenerateModelsFromLevelData(const std::string& file_name) {

	LevelData* levelData = GraphicsResourceGetter::GetLevelData(file_name);

	for (const auto& obj : levelData->objects) {

		// モデルの読み込み
		//Loader::LoadLevelModel(obj.fileName);

		// 生成
		std::unique_ptr<Model> model = std::make_unique<Model>();

		// モデル初期化（ファイル名指定）
		model->Initialize(obj.fileName);

		// 位置・回転・スケーリングを設定
		model->SetTranslate(obj.translation);
		model->SetScale(obj.scaling);

		// オイラー角からクォータニオンへ変換（ユーティリティ関数を使用）
		model->SetRotate(Math::QuaternionFromVector(obj.rotation));

		// モデルの更新
		model->Update();

		// モデルをリストに追加
		models_.push_back(std::move(model));
	}
}
// 更新
void IScene::UpdateLevelModels() {
	for (auto& model : models_) {
		model->Update();
	}
}

void IScene::DrawLevelModels(BlendMode mode) {
	for (auto& model : models_) {
		model->Draw(mode);
	}
}


