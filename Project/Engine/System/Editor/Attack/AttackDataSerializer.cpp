#include "AttackDataSerializer.h"
#include <fstream>
#include <json.hpp>

using json = nlohmann::json;

///-------------------------------------------/// 
/// 攻撃データのJson保存
///-------------------------------------------///
bool AttackDataSerializer::SaveToJson(const AttackData& data, const std::string& filepath) {
    try {
        json j;

        // 基本情報
        j["attackName"] = data.attackName;
        j["description"] = data.description;
        j["attackID"] = data.attackID;

        // タイミング設定
        j["activeDuration"] = data.activeDuration;
        j["comboWindowTime"] = data.comboWindowTime;
        j["cooldownTime"] = data.cooldownTime;

        // 共通軌道設定
        j["curveSegments"] = data.curveSegments;
        j["weaponLength"] = data.weaponLength;

        /// ===軌道チャンネルの配列=== ///
        json trajctoryChannelsArray = json::array();
        for (const auto& channel : data.trajectories) {
            json ch;
            ch["name"] = channel.name;
            ch["color"] = { channel.color.x, channel.color.y, channel.color.z };
            ch["enabled"] = channel.enabled;

            json pointsArray = json::array();
            for (const auto& point : channel.points) {
                json pointJson;
                pointJson["position"] = { point.position.x, point.position.y, point.position.z };
                pointJson["rotation"] = { point.rotation.x, point.rotation.y, point.rotation.z, point.rotation.w };
                pointJson["time"] = point.time;
                pointsArray.push_back(pointJson);
            }
            ch["points"] = pointsArray;
            trajctoryChannelsArray.push_back(ch);
        }
        j["trajectories"] = trajctoryChannelsArray;

        // コンボ設定
        j["canComboToNext"] = data.canComboToNext;
		j["nextComboID"] = data.nextComboID;
        j["branchComboIDs"] = data.branchComboIDs;

        // プレイヤーの動き
        j["moveSpeedMultiplier"] = data.moveSpeedMultiplier;
        j["rootMotion"] = { data.rootMotion.x, data.rootMotion.y, data.rootMotion.z };

        // デバッグ/プレビュー
        j["showTrajectory"] = data.showTrajectory;

		// ディレクトリが存在しない場合は作成
        std::filesystem::path dirPath = std::filesystem::path(filepath).parent_path();
        if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        // ファイルに書き込み
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file << j.dump(4);
        file.close();

        return true;

    } catch (const std::exception&) {
        return false;
    }
}

///-------------------------------------------/// 
/// JSONファイルから攻撃データを読み込み
///-------------------------------------------///
bool AttackDataSerializer::LoadFromJson(AttackData& data, const std::string& filepath) {
    try {
        // ファイル存在チェック
        if (!std::filesystem::exists(filepath)) {
            // ログ出力やエラー通知
            return false;
        }

        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;
        file.close();

        // 必須フィールドの検証
        if (!j.contains("attackName") || !j.contains("attackID")) {
            // 不正なJSONフォーマット
            return false;
        }

        // 基本情報
        data.attackName = j.value("attackName", "Unknown");
        data.description = j.value("description", "");
        data.attackID = j.value("attackID", 0);

        // タイミング設定
        data.activeDuration = j.value("activeDuration", 0.4f);
        data.comboWindowTime = j.value("comboWindowTime", 1.0f);
        data.cooldownTime = j.value("cooldownTime", 0.3f);

		// 共通軌道設定
        data.curveSegments = j.value("curveSegments", AttackData::kDefaultBezierSegments);
        data.weaponLength = j.value("weaponLength", AttackData::kDefaultWeaponLength);
        
        /// ===軌道チャンネル配列=== ///
        data.trajectories.clear();
        if (j.contains("trajectories")) {
            for (const auto& ch : j["trajectories"]) {
                TrajectoryChannel channel;
                channel.name = ch.value("name", "Channel");
                channel.enabled = ch.value("enabled", true);

                if (ch.contains("color")) {
                    channel.color.x = ch["color"][0];
					channel.color.y = ch["color"][1];
					channel.color.z = ch["color"][2];
                }

                channel.points.clear();
                if (ch.contains("points")) {
                    for (const auto& pointJson : ch["points"]) {
                        MiiEngine::BezierControlPointData point;
                        point.position.x = pointJson["position"][0];
                        point.position.y = pointJson["position"][1];
                        point.position.z = pointJson["position"][2];
                        point.rotation.x = pointJson["rotation"][0];
                        point.rotation.y = pointJson["rotation"][1];
                        point.rotation.z = pointJson["rotation"][2];
                        point.rotation.w = pointJson["rotation"][3];
                        point.time = pointJson["time"];
                        channel.points.push_back(point);
                    }
                }
                data.trajectories.push_back(channel);
            }
        }

        // コンボ設定
        data.canComboToNext = j.value("canComboToNext", false);
        data.nextComboID = j.value("nextComboID", -1);
        if (j.contains("branchComboIDs")) {
            data.branchComboIDs = j["branchComboIDs"].get<std::vector<int>>();
        }

        // プレイヤーの動き
        data.moveSpeedMultiplier = j.value("moveSpeedMultiplier", 0.4f);
        if (j.contains("rootMotion")) {
            data.rootMotion.x = j["rootMotion"][0];
            data.rootMotion.y = j["rootMotion"][1];
            data.rootMotion.z = j["rootMotion"][2];
        }

        // デバッグ/プレビュー
        data.showTrajectory = j.value("showTrajectory", true);

        return true;

    } catch (const std::exception&) {
        return false;
    }
}