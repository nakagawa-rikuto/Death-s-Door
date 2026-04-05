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

        // 軌道設定
        j["isRightHandAttack"] = data.isRightHandAttack;
        j["isLeftHandAttack"] = data.isLeftHandAttack;
        j["curveSegments"] = data.curveSegments;
        j["weaponLength"] = data.weaponLength;

		// 武器用ベジェ曲線の制御点
        json trajectoryArray = json::array();
        for (const auto& point : data.weaponTrajectoryPoints) {
            json pointJson;
            pointJson["position"] = { point.position.x, point.position.y, point.position.z };
            pointJson["rotation"] = { point.rotation.x, point.rotation.y, point.rotation.z, point.rotation.w };
            pointJson["time"] = point.time;
            trajectoryArray.push_back(pointJson);
        }
        j["weaponTrajectoryPoints"] = trajectoryArray;

		// 右手用ベジェ曲線の制御点
        json rightHandArray = json::array();
        for (const auto& point : data.rightHandTrajectoryPoints) {
            json pointJson;
            pointJson["position"] = { point.position.x, point.position.y, point.position.z };
            pointJson["rotation"] = { point.rotation.x, point.rotation.y, point.rotation.z, point.rotation.w };
            pointJson["time"] = point.time;
            rightHandArray.push_back(pointJson);
        }
        j["rightHandTrajectoryPoints"] = rightHandArray;

		// 左手用ベジェ曲線の制御点
        json leftHandArray = json::array();
        for (const auto& point : data.leftHandTrajectoryPoints) {
            json pointJson;
            pointJson["position"] = { point.position.x, point.position.y, point.position.z };
            pointJson["rotation"] = { point.rotation.x, point.rotation.y, point.rotation.z, point.rotation.w };
            pointJson["time"] = point.time;
            leftHandArray.push_back(pointJson);
        }
        j["leftHandTrajectoryPoints"] = leftHandArray;

        // コンボ設定
        j["canComboToNext"] = data.canComboToNext;
        j["nextComboID"] = data.nextComboID;
        j["branchComboIDs"] = data.branchComboIDs;

        // エフェクト設定
        j["particleEffectName"] = data.particleEffectName;
        j["particleOffset"] = { data.particleOffset.x, data.particleOffset.y, data.particleOffset.z };
        j["cameraShakeIntensity"] = data.cameraShakeIntensity;
        j["cameraShakeDuration"] = data.cameraShakeDuration;

        // サウンド設定
        j["swingSoundName"] = data.swingSoundName;
        j["hitSoundName"] = data.hitSoundName;

        // プレイヤーの動き
        j["moveSpeedMultiplier"] = data.moveSpeedMultiplier;
        j["rootMotion"] = { data.rootMotion.x, data.rootMotion.y, data.rootMotion.z };

        // デバッグ/プレビュー
        j["showTrajectory"] = data.showTrajectory;
        j["trajectoryColor"] = { data.weaponColor.x, data.weaponColor.y, data.weaponColor.z };
        j["rightHandColor"] = { data.rightHandColor.x, data.rightHandColor.y, data.rightHandColor.z };
        j["leftHandColor"] = { data.leftHandColor.x, data.leftHandColor.y, data.leftHandColor.z };

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
        file << j.dump(4); // インデント4でフォーマット
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

        // 軌道設定
        data.isRightHandAttack = j.value("isRightHandAttack", false);
        data.isLeftHandAttack = j.value("isLeftHandAttack", false);
        data.curveSegments = j.value("curveSegments", AttackData::kDefaultBezierSegments);
        data.weaponLength = j.value("weaponLength", AttackData::kDefaultWeaponLength);

		// 武器用ベジェ曲線の制御点
        data.weaponTrajectoryPoints.clear();
        if (j.contains("weaponTrajectoryPoints")) {
            for (const auto& pointJson : j["weaponTrajectoryPoints"]) {
                MiiEngine::BezierControlPointData point;
                point.position.x = pointJson["position"][0];
                point.position.y = pointJson["position"][1];
                point.position.z = pointJson["position"][2];
				point.rotation.x = pointJson["rotation"][0];
				point.rotation.y = pointJson["rotation"][1];
				point.rotation.z = pointJson["rotation"][2];
				point.rotation.w = pointJson["rotation"][3];
                point.time = pointJson["time"];
                data.weaponTrajectoryPoints.push_back(point);
            }
        }

		// 右手用ベジェ曲線の制御点
        data.rightHandTrajectoryPoints.clear();
        if (j.contains("rightHandTrajectoryPoints")) {
            for (const auto& pointJson : j["rightHandTrajectoryPoints"]) {
                MiiEngine::BezierControlPointData point;
                point.position.x = pointJson["position"][0];
                point.position.y = pointJson["position"][1];
                point.position.z = pointJson["position"][2];
                point.rotation.x = pointJson["rotation"][0];
                point.rotation.y = pointJson["rotation"][1];
                point.rotation.z = pointJson["rotation"][2];
                point.rotation.w = pointJson["rotation"][3];
                point.time = pointJson["time"];
                data.rightHandTrajectoryPoints.push_back(point);
            }
        }

		// 左手用ベジェ曲線の制御点
        data.leftHandTrajectoryPoints.clear();
        if (j.contains("leftHandTrajectoryPoints")) {
            for (const auto& pointJson : j["leftHandTrajectoryPoints"]) {
                MiiEngine::BezierControlPointData point;
                point.position.x = pointJson["position"][0];
                point.position.y = pointJson["position"][1];
                point.position.z = pointJson["position"][2];
                point.rotation.x = pointJson["rotation"][0];
                point.rotation.y = pointJson["rotation"][1];
                point.rotation.z = pointJson["rotation"][2];
                point.rotation.w = pointJson["rotation"][3];
                point.time = pointJson["time"];
                data.leftHandTrajectoryPoints.push_back(point);
            }
        }

        // コンボ設定
        data.canComboToNext = j.value("canComboToNext", false);
        data.nextComboID = j.value("nextComboID", -1);
        if (j.contains("branchComboIDs")) {
            data.branchComboIDs = j["branchComboIDs"].get<std::vector<int>>();
        }

        // エフェクト設定
        data.particleEffectName = j.value("particleEffectName", "");
        if (j.contains("particleOffset")) {
            data.particleOffset.x = j["particleOffset"][0];
            data.particleOffset.y = j["particleOffset"][1];
            data.particleOffset.z = j["particleOffset"][2];
        }
        data.cameraShakeIntensity = j.value("cameraShakeIntensity", 0.0f);
        data.cameraShakeDuration = j.value("cameraShakeDuration", 0.0f);

        // サウンド設定
        data.swingSoundName = j.value("swingSoundName", "");
        data.hitSoundName = j.value("hitSoundName", "");

        // プレイヤーの動き
        data.moveSpeedMultiplier = j.value("moveSpeedMultiplier", 0.4f);
        if (j.contains("rootMotion")) {
            data.rootMotion.x = j["rootMotion"][0];
            data.rootMotion.y = j["rootMotion"][1];
            data.rootMotion.z = j["rootMotion"][2];
        }

        // デバッグ/プレビュー
        data.showTrajectory = j.value("showTrajectory", true);
        if (j.contains("weaponColor")) {
            data.weaponColor.x = j["weaponColor"][0];
            data.weaponColor.y = j["weaponColor"][1];
            data.weaponColor.z = j["weaponColor"][2];
        }
        if (j.contains("rightHandColor")) {
            data.rightHandColor.x = j["rightHandColor"][0];
            data.rightHandColor.y = j["rightHandColor"][1];
            data.rightHandColor.z = j["rightHandColor"][2];
        }
        if (j.contains("leftHandColor")) {
            data.leftHandColor.x = j["leftHandColor"][0];
            data.leftHandColor.y = j["leftHandColor"][1];
            data.leftHandColor.z = j["leftHandColor"][2];
        }

        return true;

    } catch (const std::exception&) {
        return false;
    }
}