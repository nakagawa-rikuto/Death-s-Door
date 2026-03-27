#include "LevelManager.h"
// c++
#include <thread>
#include <cassert>
// Math
#include "Math/sMath.h"

namespace MiiEngine {
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
		std::unique_ptr<LevelData> levelData = std::make_unique<LevelData>();

        // "objects"の全オブジェクトを走査
        if (deserialized.contains("objects")) {
            LoadObjectRecursive(deserialized["objects"], levelData.get());
        }

        // レベルデータをマップに格納 
        m_objectMap[file_path] = std::move(levelData);
    }

    ///-------------------------------------------/// 
    /// 走査関数
    ///-------------------------------------------///
    void LevelManager::LoadObjectRecursive(nlohmann::json& obj, LevelData* levelData) {

        for (nlohmann::json& object : obj) {
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
                    objectData.classType = LevelData::ClassTypeLevel::None;
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

                /// ===Collider情報=== /// 
                if (object.contains("collider")) {
                    // コライダー情報の読み込み
                    nlohmann::json& collider = object["collider"];

                    // コライダータイプの取得
                    if (collider.contains("type")) {
                        std::string colliderTypeStr = collider["type"].get<std::string>();
                        objectData.colliderType = StringToColliderType(colliderTypeStr);
                    } else {
                        objectData.colliderType = LevelData::ColliderTypeLevel::None;
                    }

                    // collider_info1の読み込み (OBB: Center, AABB: Min, Sphere: Center)
                    if (collider.contains("info1")) {
                        objectData.colliderInfo1.x = (float)collider["info1"][0];
                        objectData.colliderInfo1.y = (float)collider["info1"][2];
                        objectData.colliderInfo1.z = (float)collider["info1"][1];
                    }

                    // collider_info2の読み込み (OBB: Size, AABB: Max, Sphere: Radius)
                    if (collider.contains("info2")) {
                        objectData.colliderInfo2.x = (float)collider["info2"][0];
                        objectData.colliderInfo2.y = (float)collider["info2"][2];
                        objectData.colliderInfo2.z = (float)collider["info2"][1];
                    }
                } else {
                    // コライダー情報がない場合
                    objectData.colliderType = LevelData::ColliderTypeLevel::None;
                }

                // 再帰処理（子供がいる場合）
                if (object.contains("children")) {
                    LoadObjectRecursive(object["children"], levelData);
                }
            }
        }
    }

    ///-------------------------------------------/// 
    /// 文字列からClassTypeに変換
    ///-------------------------------------------///
    LevelData::ClassTypeLevel LevelManager::StringToClassType(const std::string& str) {
        if (str == "NoClass")   return LevelData::ClassTypeLevel::None;
        // Player
        if (str == "Player")   return LevelData::ClassTypeLevel::Player;
        // Enemy
        if (str == "Enemy")    return LevelData::ClassTypeLevel::Enemy;
        // Object
        if (str == "Object")   return LevelData::ClassTypeLevel::Object;
        // Ground
        if (str == "Ground")   return LevelData::ClassTypeLevel::Ground;
        // デフォルトは None
        return LevelData::ClassTypeLevel::None;
    }

    ///-------------------------------------------/// 
    /// 文字列からColliderTypeに変換
    ///-------------------------------------------///
    LevelData::ColliderTypeLevel LevelManager::StringToColliderType(const std::string& str) {
        // OBB
        if (str == "OBB")    return LevelData::ColliderTypeLevel::OBB;
        // AABB
        if (str == "AABB")   return LevelData::ColliderTypeLevel::AABB;
        // Sphere
        if (str == "SPHERE") return LevelData::ColliderTypeLevel::Sphere;
        // デフォルトは None
        return LevelData::ColliderTypeLevel::None;
    }

    ///-------------------------------------------/// 
    /// レベルデータの取得
    ///-------------------------------------------///
    LevelData* LevelManager::GetLevelData(const std::string& file_path) {
        assert(m_objectMap.contains(file_path));
        return m_objectMap.at(file_path).get();
    }
}

