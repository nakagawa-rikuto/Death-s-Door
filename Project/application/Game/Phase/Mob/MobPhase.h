#pragma once
/// ===Include=== ///
// AABB
#include "Engine/DataInfo/ColliderData.h"
// C++
#include <vector>
#include <string>

/// ===前方宣言=== ///
class  EnemyManager;
namespace MiiEngine {
	class ParticleGroup;
}

///=====================================================/// 
/// モブのフェーズ
/// モブフェーズの内容。(全部で３ウェーブ)
///=====================================================///
class MobPhase {
public:

	MobPhase() = default;
	~MobPhase() = default;

	/// <summary>
	/// AABBを使用して初期化します。
	/// </summary>
	/// <param name="aabb">初期化に使用する軸平行境界ボックス。</param>
	void Initialize(EnemyManager* enemyManager);

	/// <summary>
	/// 更新を実行します。
	/// </summary>
	void Update();

public: /// ===Getter=== ///
	/// <summary>
	/// クリア済みかどうかを返します。
	/// </summary>
	/// <returns>クリア済みの場合はtrue</returns>
	bool IsCleared() const { return currentPhase_ == PhaseState::Cleared; }

private:

	/// ===EnemyManager=== ///
	EnemyManager* enemyManager_ = nullptr;

	/// ===ParticleGroup=== ///
	std::vector<MiiEngine::ParticleGroup*> spawnParticles_;

	/// ===PhaseState=== ///
	enum class PhaseState {
		Wave1,	// 1ウェーブ目
		Wave2,	// 2ウェーブ目
		Wave3,	// 3ウェーブ目
		Cleared // クリア
	};
	PhaseState currentPhase_ = PhaseState::Wave1;

	// 現在のウェーブが開始しているかどうか
	bool isWaveStarted_ = false;

	/// ===敵出現タイマー=== ///
	// 敵の出現までの時間
	float enemySpawnTimer_ = 0.0f;
	// 敵の出現間隔
	float enemySpawnInterval_ = 5.0f;

private:

	/// <summary>
	/// 指定された数の敵を出現させます。
	/// </summary>
	/// <param name="json_name">出現させる敵のレベルデータ</param>
	void SpawnWave(const std::string& json_name);
};

