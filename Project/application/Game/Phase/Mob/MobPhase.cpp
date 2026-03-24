#include "MobPhase.h"
// EnemyManager
#include "application/Game/Entity/Enemy/Base/EnemyManager.h"
// Service
#include "Service/GraphicsResourceGetter.h"
#include "Service/Particle.h"
#include "Service/DeltaTime.h"
// C++
#include <random>
// Math
#include "Math/sMath.h"

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
MobPhase::~MobPhase() {
	enemyManager_ = nullptr;
}

///-------------------------------------------/// 
/// 初期化
///-------------------------------------------///
void MobPhase::Initialize(EnemyManager* enemyManager) {
	enemyManager_ = enemyManager;
	currentPhase_ = PhaseState::Wave1;
	isWaveStarted_ = false;
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void MobPhase::Update() {
	// 層リターン
	if (!enemyManager_ || currentPhase_ == PhaseState::Cleared) return;

	// ウェーブが開始していなければ敵を出現させる
	if (!isWaveStarted_) {
		std::string levelData; // レベルデータを取得する処理を追加
		switch (currentPhase_) { // Wave1が敵が2体、Wave2が敵が4体, Wave3がボス（jsonの内容）
		case PhaseState::Wave1: levelData = "Level/Wave3.json"; break; // Wave3がボス
		case PhaseState::Wave2: levelData = "Level/Wave1.json"; break;
		case PhaseState::Wave3: levelData = "Level/Wave2.json"; break;
		default: break;
		}

		SpawnWave(levelData);
		isWaveStarted_ = true;
	}

	// 敵が全て倒されたら次のフェーズへ移行
	if (isWaveStarted_ && enemyManager_->GetTotalEnemyCount() == 0) {

		enemySpawnTimer_ -= Service::DeltaTime::GetDeltaTime();
		if (enemySpawnTimer_ <= 0.0f) {
			// 現在のフェーズを次のフェーズに更新
			if (currentPhase_ != PhaseState::Cleared) {
				currentPhase_ = static_cast<PhaseState>(static_cast<int>(currentPhase_) + 1);
			}
			// ウェーブ開始フラグをリセット
			isWaveStarted_ = false;
			// 敵出現タイマーをリセット
			enemySpawnTimer_ = enemySpawnInterval_;
		}
	}
}

///-------------------------------------------/// 
/// ウェーブの出現処理
///-------------------------------------------///
void MobPhase::SpawnWave(const std::string& json_name) {
	if (!enemyManager_) return;

	LevelData* levelData = Service::GraphicsResourceGetter::GetLevelData(json_name);

	// Entity分だけ回す
	for (const auto& obj : levelData->objects) {
		if (obj.fileName == "Close") {
			// Enemyの生成
			enemyManager_->Spawn(EnemyType::CloseRange, obj.translation, Math::QuaternionFromVector(obj.rotation));
		} else if (obj.fileName == "Long") {
			// Enemyの生成
			enemyManager_->Spawn(EnemyType::LongRange, obj.translation, Math::QuaternionFromVector(obj.rotation));
		} else if (obj.fileName == "Boss") {
			// Bossの生成
			enemyManager_->Spawn(EnemyType::Boss, obj.translation, Math::QuaternionFromVector(obj.rotation));
		}
	}
}