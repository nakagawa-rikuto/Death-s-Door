#pragma once
/// ===Include=== ///
#include "application/Game/Object/StageObject/StageObject.h"
#include "application/Game/Object/GameGround/Ground.h"
#include "application/Game/Object/GameGround/GroundOcean.h"
//C++
#include <string>
#include <vector>
#include <memory>

///-------------------------------------------/// 
/// GameStage
///-------------------------------------------///
class GameStage {
public:

	GameStage() = default;
	~GameStage();

	/// <summary>
	/// レベルデータを使用して初期化を行います。
	/// </summary>
	/// <param name="levelData">初期化に使用するレベルデータを含む文字列への const 参照。</param>
	void Initialize(const std::string& levelData);

	/// <summary>
	/// オブジェクトやシステムの状態を更新するための関数。
	/// </summary>
	void Update();

	/// <summary>
	/// 指定したブレンドモードで描画を行う関数。デフォルトは BlendMode::kBlendModeAdd。
	/// </summary>
	/// <param name="mode">描画に使用するブレンドモード。省略した場合は BlendMode::kBlendModeAdd が使用されます。</param>
	void Draw(MiiEngine::BlendMode mode = MiiEngine::BlendMode::KBlendModeNormal);

private:
	// ステージオブジェクトのリスト
	std::vector<std::shared_ptr<StageObject>> objects_; 
	std::vector<std::shared_ptr<GroundOcean>> Oceans_;
	std::vector<std::shared_ptr<Ground>> grounds_;

private:

	/// <summary>
	/// ステージデータを読み込む関数。
	/// </summary>
	/// <param name="stageData">読み込むステージデータを含む文字列への参照。ステージ定義や構成を表すテキストを渡します。</param>
	void LoadStageData(const std::string& stageData);
};

