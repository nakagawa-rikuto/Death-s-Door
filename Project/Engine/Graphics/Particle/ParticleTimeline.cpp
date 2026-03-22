#include "ParticleTimeline.h"

namespace MiiEngine {
    ///-------------------------------------------/// 
    /// ToJson
    ///-------------------------------------------///
    nlohmann::json ParticleTimelineSerializer::ToJson(const ParticleTimeline& timeline) {
        nlohmann::json j;
        j["type"] = "timeline";
        j["name"] = timeline.name;
        j["totalDuration"] = timeline.totalDuration;

        nlohmann::json entriesJson = nlohmann::json::array();
        for (const auto& entry : timeline.entries) {
            nlohmann::json entryJson;
            entryJson["particleName"] = entry.particleName;
            entryJson["startTime"] = entry.startTime;
            entryJson["offset"] = Vector3ToJson(entry.offset);
            entriesJson.push_back(entryJson);
        }
        j["entries"] = entriesJson;

        return j;
    }

    ///-------------------------------------------/// 
    /// FromJson
    ///-------------------------------------------///
    ParticleTimeline ParticleTimelineSerializer::FromJson(const nlohmann::json& json) {
        ParticleTimeline timeline;

        if (json.contains("name")) timeline.name = json["name"];
        if (json.contains("totalDuration")) timeline.totalDuration = json["totalDuration"];

        if (json.contains("entries") && json["entries"].is_array()) {
            for (const auto& entryJson : json["entries"]) {
                ParticleTimelineEntry entry;
                if (entryJson.contains("particleName")) entry.particleName = entryJson["particleName"];
                if (entryJson.contains("startTime")) entry.startTime = entryJson["startTime"];
                if (entryJson.contains("offset")) entry.offset = JsonToVector3(entryJson["offset"]);
                timeline.entries.push_back(entry);
            }
        }

        return timeline;
    }

    ///-------------------------------------------/// 
    /// Vector3のJSON変換
    ///-------------------------------------------///
    nlohmann::json ParticleTimelineSerializer::Vector3ToJson(const Vector3& vec) {
        return nlohmann::json::array({ vec.x, vec.y, vec.z });
    }
    Vector3 ParticleTimelineSerializer::JsonToVector3(const nlohmann::json& json) {
        if (json.is_array() && json.size() >= 3) {
            return Vector3{ json[0], json[1], json[2] };
        }
        return Vector3{ 0.0f, 0.0f, 0.0f };
    }
}
