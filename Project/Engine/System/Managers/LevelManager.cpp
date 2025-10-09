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

            /// ===FileName=== ///
            if (object.contains("file_name")) {
                // ファイル名
                objectData.fileName = object["file_name"];
            }

            /// ===ClassName=== ///
            if (object.contains("class_name")) {
                std::string classStr = object["class_name"].get<std::string>();
                objectData.classType = StringToClassType(classStr);
            } else {
                objectData.classType = LevelData::ClassType::None;
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
/// 文字列からClassTypeに変換
///-------------------------------------------///
LevelData::ClassType LevelManager::StringToClassType(const std::string& str) {
    if (str == "NoClass")   return LevelData::ClassType::None;

    if (str == "Player1")   return LevelData::ClassType::Player1;
    if (str == "Player2")   return LevelData::ClassType::Player2;

    if (str == "Enemy1")    return LevelData::ClassType::Enemy1;
    if (str == "Enemy2")    return LevelData::ClassType::Enemy2;
    if (str == "Enemy3")    return LevelData::ClassType::Enemy3;
    if (str == "Enemy4")    return LevelData::ClassType::Enemy4;
    if (str == "Enemy5")    return LevelData::ClassType::Enemy5;
    if (str == "Enemy6")    return LevelData::ClassType::Enemy6;
    if (str == "Enemy7")    return LevelData::ClassType::Enemy7;
    if (str == "Enemy8")    return LevelData::ClassType::Enemy8;
    if (str == "Enemy9")    return LevelData::ClassType::Enemy9;
    if (str == "Enemy10")   return LevelData::ClassType::Enemy10;

    if (str == "Object1")   return LevelData::ClassType::Object1;
    if (str == "Object2")   return LevelData::ClassType::Object2;
    if (str == "Object3")   return LevelData::ClassType::Object3;
    if (str == "Object4")   return LevelData::ClassType::Object4;
    if (str == "Object5")   return LevelData::ClassType::Object5;
    if (str == "Object6")   return LevelData::ClassType::Object6;
    if (str == "Object7")   return LevelData::ClassType::Object7;
    if (str == "Object8")   return LevelData::ClassType::Object8;
    if (str == "Object9")   return LevelData::ClassType::Object9;
    if (str == "Object10")  return LevelData::ClassType::Object10;

    if (str == "Ground1")   return LevelData::ClassType::Ground1;
    if (str == "Ground2")   return LevelData::ClassType::Ground2;
    if (str == "Ground3")   return LevelData::ClassType::Ground3;
    if (str == "Ground4")   return LevelData::ClassType::Ground4;
    if (str == "Ground5")   return LevelData::ClassType::Ground5;
    if (str == "Ground6")   return LevelData::ClassType::Ground6;
    if (str == "Ground7")   return LevelData::ClassType::Ground7;
    if (str == "Ground8")   return LevelData::ClassType::Ground8;
    if (str == "Ground9")   return LevelData::ClassType::Ground9;
    if (str == "Ground10")  return LevelData::ClassType::Ground10;

    if (str == "SkyDome1")  return LevelData::ClassType::SkyBox1;
    if (str == "SkyDome2")  return LevelData::ClassType::SkyBox2;
    if (str == "SkyDome3")  return LevelData::ClassType::SkyBox3;
    if (str == "SkyDome4")  return LevelData::ClassType::SkyBox4;
    if (str == "SkyDome5")  return LevelData::ClassType::SkyBox5;
    if (str == "SkyDome6")  return LevelData::ClassType::SkyBox6;
    if (str == "SkyDome7")  return LevelData::ClassType::SkyBox7;
    if (str == "SkyDome8")  return LevelData::ClassType::SkyBox8;
    if (str == "SkyDome9")  return LevelData::ClassType::SkyBox9;
    if (str == "SkyDome10") return LevelData::ClassType::SkyBox10;

    // デフォルトは None
    return LevelData::ClassType::None;
}

///-------------------------------------------/// 
/// レベルデータの取得
///-------------------------------------------///
LevelData* LevelManager::GetLevelData(const std::string& file_path) {
    assert(m_objectMap.contains(file_path));
    return m_objectMap.at(file_path);
}


