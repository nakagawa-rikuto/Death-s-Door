#include "LevelManager.h"
// c++
#include <thread>
#include <cassert>
// Math
#include "Math/sMath.h"

///-------------------------------------------/// 
/// Jsonの読み込み関数
///-------------------------------------------///
void LevelManager::LoadLevelJson(const std::string& basePath, const std::string& file_path) {

    /// ===JSONファイルを読み込んでみる=== ///
    // ファイルストリーム
    std::ifstream file;

	// basePath`と`file_path`を結合して完全なパスを作成
	std::string full_path = basePath + "/" + file_path;

    // ファイルを開く
    file.open(full_path);

    // ファイルオープン失敗をチェック
    if (file.fail()) {
        assert(0);
    }

    /// ===ファイルチェック=== ///
    // JSON文字列から解凍したデータ
    nlohmann::json deserialized;

    // 解凍
    file >> deserialized;

    // 正しいレベルデータファイルかチェック
    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    // "name"を文字列として取得
    std::string name =
        deserialized["name"].get<std::string>();
    // 正しいレベルデータファイルかチェック
    assert(name.compare("scene") == 0);

    /// ===オブジェクトの走査=== ///
    // レベルデータ格納用インスタンスを生成
    LevelData* levelData = new LevelData();

    // "objects"の全オブジェクトを走査
    LoadobjectRecursive(deserialized, levelData);

	// レベルデータをマップに格納 
    m_objectMap[file_path] = std::move(levelData);
}
// 走査関数
void LevelManager::LoadobjectRecursive(nlohmann::json object, LevelData* levelData) {

    for (nlohmann::json& object : object["objects"]) {
        assert(object.contains("type"));

        // 無効オプション
        if (object.contains("disabled")) {
            // 有効無効フラグ
            bool disabled = object["disabled"].get<bool>();
            if (disabled) {
                // 配置しない
                continue;
            }
        }

        // 種別を取得
        std::string type = object["type"].get<std::string>();

        // 種類ごとの処理
        /// ===メッシュの読み込み=== ///
        // MESH
        if (type.compare("MESH") == 0) {
            // 要素追加
            levelData->objects.emplace_back(LevelData::JsonObjectData{});
            // 今追加した要素の参照を得る
            LevelData::JsonObjectData& objectData = levelData->objects.back();

            if (object.contains("file_name")) {
                // ファイル名
                objectData.fileName = object["file_name"];
            }

            /// ===トランスフォームのパラメータ=== ///
            // トランスフォームのパラメータ読み込み
            nlohmann::json& transform = object["transform"];

            // 平行移動
            objectData.translation.x = (float)transform["translation"][0];
            objectData.translation.y = (float)transform["translation"][2];
            objectData.translation.z = (float)transform["translation"][1];

            // 回転角
            objectData.rotation.x = -(float)transform["rotation"][0];
            objectData.rotation.y = -(float)transform["rotation"][2];
            objectData.rotation.z = -(float)transform["rotation"][1];

            // スケーリング
            objectData.scaling.x = (float)transform["scaling"][0];
            objectData.scaling.y = (float)transform["scaling"][2];
            objectData.scaling.z = (float)transform["scaling"][1];
        }

        // 再帰処理（子供がいる場合）
        if (object.contains("children")) {
            LoadobjectRecursive(object, levelData);
        }
    }
}

///-------------------------------------------/// 
/// レベルデータの取得
///-------------------------------------------///
LevelData* LevelManager::GetLevelData(const std::string& file_path) {
    assert(m_objectMap.contains(file_path));
    return m_objectMap.at(file_path);
}


