#pragma once
/// ===Include=== ///
// C++
#include <iostream>
#include <fstream>
#include <map>
// JSON
#include <json.hpp>

// Data
#include "Engine/DataInfo/LevelData.h"


///=====================================================/// 
/// LevelManager
///=====================================================///
class LevelManager {
public:

	LevelManager() = default;
	~LevelManager() = default;

	// Load
	void LoadLevelJson(const std::string& basePath, const std::string& file_path);

	// レベルデータの取得
	LevelData* GetLevelData(const std::string& file_path);

private:

	// 格納データ
	std::map<std::string, LevelData*> m_objectMap;

private:

	// 再帰用関数
	void LoadobjectRecursive(nlohmann::json object, LevelData* levelData);
	// ClassType変換関数
	static LevelData::ClassType StringToClassType(const std::string& str);
};

