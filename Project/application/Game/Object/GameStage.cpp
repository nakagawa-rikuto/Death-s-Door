#include "GameStage.h"
// Service
#include "Service/GraphicsResourceGetter.h"
// Math
#include "Math/sMath.h"
// ImGui
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
GameStage::~GameStage() {
	objects_.clear();
	Oceans_.clear();
	grounds_.clear();
}

///-------------------------------------------/// 
/// 初期化　
///-------------------------------------------///
void GameStage::Initialize(const std::string& levelData) {

	// ステージデータを読み込む
	LoadStageData(levelData);
}

///-------------------------------------------/// 
/// 更新
///-------------------------------------------///
void GameStage::Update() {

	// GroundOceanの更新
	for (const auto& ocean : Oceans_) {
		if (ocean) {
#ifdef USE_IMGUI
			ocean->ShowImGui();
#endif // USE_IMGUI

			ocean->Update();
		}
	}

	// Groundの更新
	for (const auto& ground : grounds_) {
		if (ground) {
			ground->Update();
		}
	}

	// オブジェクトの更新
	for (const auto& obj : objects_) {
		if (obj) {
			obj->Update();
		}
	}
}

///-------------------------------------------/// 
/// 描画
///-------------------------------------------///
void GameStage::Draw(MiiEngine::BlendMode mode) {

	// GroundOceanの更新
	for (const auto& ocean : Oceans_) {
		if (ocean) {
			ocean->Draw();
		}
	}

	// GroundOceanの更新
	for (const auto& ground : grounds_) {
		if (ground) {
			ground->Draw(mode);
		}
	}

	// オブジェクトの描画
	for (const auto& obj : objects_) {
		if (obj) {
			obj->Draw(mode);
		}
	}
}

///-------------------------------------------/// 
/// ステージデータを読み込む関数
///-------------------------------------------///
void GameStage::LoadStageData(const std::string& stageData) {
	// レベルデータの取得
	LevelData* levelData = Service::GraphicsResourceGetter::GetLevelData(stageData);

	// オブジェクト分回す
	for (const auto& stage : levelData->objects) {
		if (stage.classType == LevelData::ClassTypeLevel::Ground) {

			if (stage.fileName == "Ocean") {
				std::unique_ptr<GroundOcean> ocean = std::make_unique<GroundOcean>();
				ocean->Initialize();
				ocean->Update();
				Oceans_.emplace_back(std::move(ocean));
			} else {
				// Object3dの生成
				std::unique_ptr<Ground> ground = std::make_unique<Ground>();
				ground->GameInit(stage.fileName);
				// 座標設定
				ground->SetTranslate(stage.translation);
				ground->SetRotate(Math::QuaternionFromVector(stage.rotation));
				ground->SetScale(stage.scaling);
				// HalfSizeの設定
				ground->SetHalfSize(stage.colliderInfo2 * 0.5f);
				// 一回更新
				ground->Update();
				// 配列に追加
				grounds_.emplace_back(std::move(ground));
			}
			continue;
		} else if (stage.classType == LevelData::ClassTypeLevel::Object) {
			std::unique_ptr<StageObject> object = std::make_unique<StageObject>();
			object->GameInit(stage.fileName);
			// Transformを設定
			object->SetTranslate(stage.translation);
			object->SetRotate(Math::QuaternionFromVector(stage.rotation));
			object->SetScale(stage.scaling);
			// 一回更新
			object->Update();
			// 配列に追加
			objects_.emplace_back(std::move(object));
			continue;
		} else {
			// その他のクラスは無視
			continue;
		}
	}
}
